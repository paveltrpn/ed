#include "renderjob.h"

#include <cmath>
#include <exception>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "dsbtt/bvh.h"
#include "dsbtt/camera.h"
#include "dsbtt/sparseaccelerator.h"
#include "dsbtt/tracer.h"
#include "tcpbridge/renderprotocol.h"

namespace tire {

namespace {

constexpr size_t kSamplesRoot{ 4 };
constexpr size_t kTilesRoot{ 4 };
constexpr size_t kThreadCount{ kTilesRoot * kTilesRoot };

}  // namespace

RenderJob::RenderJob( uint32_t width, uint32_t height, std::string sceneObj, RenderCamera camera )
    : _width{ width }
    , _height{ height }
    , _sceneObj{ std::move( sceneObj ) }
    , _camera{ camera }
    , _image( tire::ImageMessage::pixelCount( width, height ), 0.0f ) {
}

RenderJob::~RenderJob() {
    cancel();
}

auto RenderJob::start() -> void {
    if ( _worker.joinable() ) {
        return;
    }

    _worker = std::thread{ [this]() -> void { run(); } };
}

auto RenderJob::cancel() -> void {
    _cancellation = true;
    if ( _worker.joinable() ) {
        _worker.join();
    }
}

auto RenderJob::width() const noexcept -> uint32_t {
    return _width;
}

auto RenderJob::height() const noexcept -> uint32_t {
    return _height;
}

auto RenderJob::snapshot() const -> RenderJobSnapshot {
    const std::scoped_lock lock{ _snapshotMutex };
    return RenderJobSnapshot{ _state, _error, _image };
}

auto RenderJob::run() -> void {
    try {
        auto sceneStream = std::istringstream{ _sceneObj };
        Mesh mesh{ sceneStream, false };
        Bvh bvh{ mesh };
        SparseAccelerator accelerator{ bvh };
        Camera camera{};

        const auto eye = tire::algebra::vector<float, 3>{ static_cast<float>( _camera._eye[0] ),
                                                          static_cast<float>( _camera._eye[1] ),
                                                          static_cast<float>( _camera._eye[2] ) };

        const auto target = tire::algebra::vector<float, 3>{ static_cast<float>( _camera._target[0] ),
                                                             static_cast<float>( _camera._target[1] ),
                                                             static_cast<float>( _camera._target[2] ) };

        const auto up =
            tire::algebra::vector<float, 3>{ static_cast<float>( _camera._up[0] ), static_cast<float>( _camera._up[1] ),
                                             static_cast<float>( _camera._up[2] ) };

        const auto aspect = static_cast<double>( _width ) / static_cast<double>( _height );
        const auto verticalFov = tire::algebra::degToRad( _camera._fieldOfViewY );
        const auto horizontalFov = tire::algebra::radToDeg( 2.0 * std::atan( aspect * std::tan( verticalFov / 2.0 ) ) );

        camera.lookAt( eye, target, up, static_cast<float>( horizontalFov ),
                       static_cast<float>( _camera._nearDistance ), static_cast<float>( _camera._farDistance ),
                       static_cast<int>( _width ), static_cast<int>( _height ) );

        if ( _cancellation ) {
            return;
        }

        const auto pixelCount = _image.size();
        const auto sampleCount = kSamplesRoot * kSamplesRoot;
        if ( pixelCount > std::numeric_limits<size_t>::max() / sampleCount ) {
            throw std::overflow_error{ "Render sample count overflows addressable memory" };
        }

        std::vector<Tracer::HitPoint, AlignedAllocator<Tracer::HitPoint, 32>> hitPoints( pixelCount * sampleCount );
        setState( RenderJobState::Rendering );

        auto workerErrorMutex = std::mutex{};
        auto workerError = std::exception_ptr{};
        const auto captureWorkerError = [this, &workerErrorMutex, &workerError]( std::exception_ptr error ) -> void {
            {
                const std::scoped_lock lock{ workerErrorMutex };
                if ( workerError == nullptr ) {
                    workerError = std::move( error );
                }
            }
            _cancellation = true;
        };

        auto primaryThreads = std::vector<std::thread>{};
        primaryThreads.reserve( kThreadCount );
        try {
            for ( size_t tileIndex = 0; tileIndex < kThreadCount; ++tileIndex ) {
                primaryThreads.emplace_back( [this, &camera, &accelerator, &hitPoints, pixelCount, &captureWorkerError,
                                              tileIndex]() -> void {
                    try {
                        const auto width = static_cast<size_t>( _width );
                        const auto height = static_cast<size_t>( _height );
                        const auto tileX = tileIndex % kTilesRoot;
                        const auto tileY = tileIndex / kTilesRoot;
                        const auto xBegin = tileX * width / kTilesRoot;
                        const auto xEnd = ( tileX + 1 ) * width / kTilesRoot;
                        const auto yBegin = tileY * height / kTilesRoot;
                        const auto yEnd = ( tileY + 1 ) * height / kTilesRoot;

                        Random rnd{ static_cast<int>( 31251 + tileIndex ) };
                        Tracer tracer{ camera, accelerator };

                        for ( size_t sy = 0; sy < kSamplesRoot; ++sy ) {
                            for ( size_t sx = 0; sx < kSamplesRoot; ++sx ) {
                                if ( _cancellation ) {
                                    return;
                                }

                                const auto sampleIndex = sy * kSamplesRoot + sx;

                                for ( size_t y = yBegin; y < yEnd; ++y ) {
                                    for ( size_t x = xBegin; x < xEnd; ++x ) {
                                        const auto px =
                                            static_cast<float>( x ) + ( static_cast<float>( sx ) + rnd.nextFloat() ) /
                                                                          static_cast<float>( kSamplesRoot );
                                        const auto py =
                                            static_cast<float>( y ) + ( static_cast<float>( sy ) + rnd.nextFloat() ) /
                                                                          static_cast<float>( kSamplesRoot );
                                        const auto hitPointIndex = sampleIndex * pixelCount + y * width + x;
                                        hitPoints[hitPointIndex] = tracer.samplePrimary( px, py );
                                    }
                                }
                            }
                        }
                    } catch ( ... ) {
                        captureWorkerError( std::current_exception() );
                    }
                } );
            }
        } catch ( ... ) {
            _cancellation = true;
            joinThreads( primaryThreads );
            throw;
        }

        joinThreads( primaryThreads );

        if ( workerError != nullptr ) {
            std::rethrow_exception( workerError );
        }

        if ( _cancellation ) {
            return;
        }

        auto aoThreads = std::vector<std::thread>{};
        aoThreads.reserve( kThreadCount );
        try {
            for ( size_t tileIndex = 0; tileIndex < kThreadCount; ++tileIndex ) {
                aoThreads.emplace_back( [this, &camera, &accelerator, &hitPoints, pixelCount, &captureWorkerError,
                                         tileIndex]() -> void {
                    try {
                        const auto width = static_cast<size_t>( _width );
                        const auto height = static_cast<size_t>( _height );
                        const auto tileX = tileIndex % kTilesRoot;
                        const auto tileY = tileIndex / kTilesRoot;
                        const auto xBegin = tileX * width / kTilesRoot;
                        const auto xEnd = ( tileX + 1 ) * width / kTilesRoot;
                        const auto yBegin = tileY * height / kTilesRoot;
                        const auto yEnd = ( tileY + 1 ) * height / kTilesRoot;

                        auto tilePixels = std::vector<float>( ( xEnd - xBegin ) * ( yEnd - yBegin ), 0.0f );
                        Tracer tracer{ camera, accelerator };

                        for ( size_t sy = 0; sy < kSamplesRoot; ++sy ) {
                            for ( size_t sx = 0; sx < kSamplesRoot; ++sx ) {
                                if ( _cancellation ) {
                                    return;
                                }

                                const auto sampleIndex = sy * kSamplesRoot + sx;

                                for ( size_t y = yBegin; y < yEnd; ++y ) {
                                    for ( size_t x = xBegin; x < xEnd; ++x ) {
                                        const auto hitPointIndex = sampleIndex * pixelCount + y * width + x;
                                        const auto hitPoint = hitPoints[hitPointIndex];
                                        auto intensity = 1.0f;
                                        if ( hitPoint._hit ) {
                                            intensity = tracer.sampleAO( hitPoint );
                                        }

                                        const auto tilePixelIndex = ( y - yBegin ) * ( xEnd - xBegin ) + ( x - xBegin );
                                        tilePixels[tilePixelIndex] +=
                                            intensity / static_cast<float>( kSamplesRoot * kSamplesRoot );
                                    }
                                }

                                publishTile( xBegin, xEnd, yBegin, yEnd, tilePixels );
                            }
                        }
                    } catch ( ... ) {
                        captureWorkerError( std::current_exception() );
                    }
                } );
            }
        } catch ( ... ) {
            _cancellation = true;
            joinThreads( aoThreads );
            throw;
        }

        joinThreads( aoThreads );
        if ( workerError != nullptr ) {
            std::rethrow_exception( workerError );
        }
        if ( _cancellation ) {
            return;
        }

        setState( RenderJobState::Complete );
    } catch ( const std::exception& exception ) {
        fail( exception.what() );
    }
}

auto RenderJob::setState( RenderJobState state ) -> void {
    const std::scoped_lock lock{ _snapshotMutex };
    _state = state;
}

auto RenderJob::fail( const std::string& message ) -> void {
    const std::scoped_lock lock{ _snapshotMutex };
    _error = message;
    _state = RenderJobState::Failed;
}

auto RenderJob::publishTile( size_t xBegin, size_t xEnd, size_t yBegin, size_t yEnd,
                             const std::vector<float>& tilePixels ) -> void {
    const auto width = static_cast<size_t>( _width );
    const auto tileWidth = xEnd - xBegin;
    const std::scoped_lock lock{ _snapshotMutex };
    for ( size_t y = yBegin; y < yEnd; ++y ) {
        for ( size_t x = xBegin; x < xEnd; ++x ) {
            const auto sourceIndex = ( y - yBegin ) * tileWidth + ( x - xBegin );
            _image[y * width + x] = tilePixels[sourceIndex];
        }
    }
}

auto RenderJob::joinThreads( std::vector<std::thread>& threads ) -> void {
    for ( auto& thread : threads ) {
        if ( thread.joinable() ) {
            thread.join();
        }
    }
}

}  // namespace tire
