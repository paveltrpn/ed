//
//  main.cpp
//  tracer
//
//  Created by Rasmus Barringer on 2012-10-06.
//  Copyright (c) 2012 Lund University. All rights reserved.
//

#include <array>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <charconv>
#include <cmath>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <stdio.h>
#include <cstring>
#include <string_view>
#include <thread>
#include <utility>
#include <print>

#include <shared_mutex>
#include <stdexcept>

#include <nlohmann/json.hpp>

#include <uv.h>

#include "tcpbridge/renderprotocol.h"
#include "tcpbridge/tcpbridge.h"

#include "renderjob.h"

#include "dsbtt/bvh.h"
#include "dsbtt/tracer.h"
#include "dsbtt/stackaccelerator.h"
#include "dsbtt/laineaccelerator.h"
#include "dsbtt/sparseaccelerator.h"
#include "dsbtt/hapalaaccelerator.h"
#include "dsbtt/implicitaaccelerator.h"
#include "dsbtt/implicitbaccelerator.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

static double seconds() {
#ifdef _WIN32
    return GetTickCount64() * ( 1.0 / 1000.0 );
#else
    struct timeval tv;
    gettimeofday( &tv, 0 );
    return tv.tv_sec + tv.tv_usec * ( 1.0 / 1000000.0 );
#endif
}

static RayAccelerator* createAccelerator( Bvh& bvh, const char* name ) {
    std::cout << "Accelerator: " << name << "." << std::endl;

    if ( strcmp( name, "stack" ) == 0 )
        return new StackAccelerator( bvh );
    else if ( strcmp( name, "sparse" ) == 0 )
        return new SparseAccelerator( bvh );
    else if ( strcmp( name, "implicita" ) == 0 )
        return new ImplicitAAccelerator( bvh );
    else if ( strcmp( name, "implicitb" ) == 0 )
        return new ImplicitBAccelerator( bvh );
    else if ( strcmp( name, "laine0" ) == 0 )
        return new LaineAccelerator<0>( bvh );
    else if ( strcmp( name, "laine1" ) == 0 )
        return new LaineAccelerator<1>( bvh );
    else if ( strcmp( name, "laine2" ) == 0 )
        return new LaineAccelerator<2>( bvh );
    else if ( strcmp( name, "laine4" ) == 0 )
        return new LaineAccelerator<4>( bvh );
    else if ( strcmp( name, "laine8" ) == 0 )
        return new LaineAccelerator<8>( bvh );
    else if ( strcmp( name, "hapala" ) == 0 )
        return new HapalaAccelerator( bvh );

    std::cout << "ERROR: Invalid accelerator." << std::endl;
    std::cout << "Pick one of: stack, implicita, implicitb, sparse, hapala, laine0,1,2,4,8." << std::endl;

    exit( 1 );

    return 0;
}

struct ServeOptions {
    bool _serve{ false };
    bool _valid{ true };
    uint16_t _port{ 45454 };
};

static auto parseServeOptions( int argc, const char* argv[] ) -> ServeOptions {
    auto options = ServeOptions{};
    for ( int index = 1; index < argc; ++index ) {
        if ( std::string_view{ argv[index] } == "--serve" ) {
            options._serve = true;
            break;
        }
    }
    if ( !options._serve ) {
        return options;
    }

    for ( int index = 1; index < argc; ++index ) {
        const auto argument = std::string_view{ argv[index] };
        if ( argument == "--serve" ) {
            continue;
        }
        if ( argument != "--port" || index + 1 >= argc ) {
            options._valid = false;
            return options;
        }

        const auto value = std::string_view{ argv[++index] };
        auto port = 0;
        const auto conversion = std::from_chars( value.data(), value.data() + value.size(), port );
        if ( conversion.ec != std::errc{} || conversion.ptr != value.data() + value.size() || port < 1 ||
             port > 65535 ) {
            options._valid = false;
            return options;
        }
        options._port = static_cast<uint16_t>( port );
    }
    return options;
}

namespace {

// ======================================================================================
// ==================== ServeCompletion =================================================
// ======================================================================================

struct ServeCompletion {
    std::mutex _mutex{};
    std::condition_variable _condition{};
    bool _finished{ false };
};

auto messageFromString( std::string_view text ) -> std::vector<std::byte> {
    auto message = std::vector<std::byte>( text.size() );
    if ( !text.empty() ) {
        std::memcpy( message.data(), text.data(), text.size() );
    }
    return message;
}

auto payloadIs( const std::vector<std::byte>& payload, std::string_view text ) -> bool {
    if ( payload.size() != text.size() ) {
        return false;
    }

    for ( size_t index = 0; index < payload.size(); ++index ) {
        if ( payload[index] != static_cast<std::byte>( text[index] ) ) {
            return false;
        }
    }
    return true;
}

auto makeRenderImageResponse( const tire::RenderJob& job ) -> std::vector<std::byte> {
    const auto snapshot = job.snapshot();
    if ( snapshot._state == tire::RenderJobState::Failed ) {
        if ( snapshot._error.empty() ) {
            throw std::runtime_error{ "Render job failed without an error message" };
        }
        throw std::runtime_error{ snapshot._error };
    }

    auto image = tire::RenderImagePayload{};
    image._width = job.width();
    image._height = job.height();
    image._final = snapshot._state == tire::RenderJobState::Complete;
    image._pixels.resize( snapshot._pixels.size() );
    for ( size_t index = 0; index < snapshot._pixels.size(); ++index ) {
        auto value = std::clamp( snapshot._pixels[index], 0.0f, 1.0f );
        if ( !std::isfinite( value ) ) {
            value = 0.0f;
        }
        const auto gray = static_cast<uint8_t>( value * 255.0f + 0.5f );
        image._pixels[index] = static_cast<std::byte>( gray );
    }
    return tire::encodeRenderImage( image );
}

auto runServeSession( tire::TcpBridge& bridge, ServeCompletion& completion, uint16_t port ) -> tire::Task<int> {
    auto exitCode = 1;
    auto handshakeComplete = false;
    auto job = std::unique_ptr<tire::RenderJob>{};

    const auto acceptStatus = co_await bridge.accept( "127.0.0.1", port );

    if ( acceptStatus != 0 ) {
        std::cerr << "Failed to accept TCP connection";
        if ( acceptStatus != UV_ECANCELED ) {
            std::cerr << ": " << uv_strerror( acceptStatus );
        }
        std::cerr << std::endl;
    }

    while ( acceptStatus == 0 ) {
        const auto message = co_await bridge.read();

        if ( message._status == UV_EOF ) {
            exitCode = 0;
            break;
        }

        if ( message._status != 0 ) {
            if ( message._status != UV_ECANCELED ) {
                std::cerr << "TCP read failed: " << uv_strerror( message._status ) << std::endl;
            }
            break;
        }

        auto response = std::vector<std::byte>{};
        try {
            if ( !handshakeComplete ) {
                if ( !payloadIs( message._payload, "handshake_from_ed" ) ) {
                    throw std::runtime_error{ "Expected the ED handshake" };
                }

                handshakeComplete = true;
                response = messageFromString( "hello from rt" );
            } else {
                const auto type = tire::renderMessageType( message._payload );
                if ( type == tire::RenderMessageType::Request ) {
                    if ( job != nullptr ) {
                        throw std::runtime_error{ "A render request is already active" };
                    }

                    const auto request = tire::decodeRenderRequest( message._payload );
                    auto newJob = std::make_unique<tire::RenderJob>( request._width, request._height );
                    newJob->start();
                    job = std::move( newJob );
                } else if ( type == tire::RenderMessageType::Poll ) {
                    tire::decodeRenderPollRequest( message._payload );
                    if ( job == nullptr ) {
                        throw std::runtime_error{ "Cannot poll before starting a render" };
                    }
                    response = makeRenderImageResponse( *job );
                } else {
                    throw std::runtime_error{ "Unexpected message sent to the RT render server" };
                }
            }
        } catch ( const std::exception& exception ) {
            response = tire::encodeRenderError( exception.what() );
        } catch ( ... ) {
            response = tire::encodeRenderError( "Unknown RT render server failure" );
        }

        if ( !response.empty() ) {
            const auto sendStatus = co_await bridge.send( std::move( response ) );
            if ( sendStatus != 0 ) {
                if ( sendStatus != UV_ECANCELED ) {
                    std::cerr << "TCP send failed: " << uv_strerror( sendStatus ) << std::endl;
                }
                break;
            }
        }
    }

    if ( job != nullptr ) {
        job->cancel();
        job.reset();
    }

    {
        const std::scoped_lock lock{ completion._mutex };
        completion._finished = true;
    }

    completion._condition.notify_all();

    co_return exitCode;
}

}  // namespace

static auto runServe( uint16_t port ) -> int {
    auto completion = ServeCompletion{};
    auto bridge = std::make_unique<tire::TcpBridge>( tire::kRenderProtocolMaximumPayload );

    auto session = runServeSession( *bridge, completion, port );
    session.start();

    std::unique_lock lock{ completion._mutex };
    completion._condition.wait( lock, [&completion]() -> bool {
        //
        return completion._finished;
    } );
    lock.unlock();

    bridge->close();

    // Join the event loop thread before the completed session frame is destroyed by
    // ~Task: the coroutine now runs to completion on that thread.
    bridge.reset();

    if ( session.exception() ) {
        std::cerr << "Session terminated by exception" << std::endl;
        return 1;
    }

    return session.get();
}

auto main( int argc, const char* argv[] ) -> int {
    const auto serveOptions = parseServeOptions( argc, argv );
    if ( serveOptions._serve ) {
        if ( !serveOptions._valid ) {
            std::cerr << "Usage: rt --serve [--port N], where N is between 1 and 65535." << std::endl;
            return 2;
        }
        return runServe( serveOptions._port );
    }

    assert( sizeof( Triangle ) == 64 );
    assert( sizeof( Bvh::Node ) == 64 );
    assert( sizeof( Tracer::HitPoint ) == 32 );

    std::cout << "Loading obj..." << std::flush;
    Mesh mesh( "battlefield.obj", false );
    std::cout << "done." << std::endl;

    std::cout << "Building bvh..." << std::flush;
    Bvh bvh( mesh );
    std::cout << "done." << std::endl;

    const int width = 800;
    const int height = 400;
    const int samplesRoot = 4;
    const auto tilesRoot{ 4 };
    const auto threadCount{ tilesRoot * tilesRoot };

    Camera camera;
    RayAccelerator* accelerator = createAccelerator( bvh, argc > 1 ? argv[1] : "" );

    camera.lookAt( tire::algebra::vector<float, 3>( 300.0f, 100.0f, 300.0f ),
                   tire::algebra::vector<float, 3>( 0.0f, 10.0f, 0.0f ),
                   tire::algebra::vector<float, 3>( 0.0f, 1.0f, 0.0f ), 50.0f, 1.0f, 1000.0f, width, height );

    std::vector<Tracer::HitPoint, AlignedAllocator<Tracer::HitPoint, 32>> hitPoints( width * height * samplesRoot *
                                                                                     samplesRoot );
    std::vector<float> image( width * height );

    std::print( "Primary rays: " );

    double primStart = seconds();
    std::array<std::thread, threadCount> primaryThreads{};

    for ( int tileIndex{ 0 }; tileIndex < threadCount; ++tileIndex ) {
        primaryThreads[tileIndex] = std::thread{
            [&camera, accelerator, &hitPoints, width, height, samplesRoot, tilesRoot, tileIndex]() -> void {
                const auto tileX{ tileIndex % tilesRoot };
                const auto tileY{ tileIndex / tilesRoot };
                const auto xBegin{ tileX * width / tilesRoot };
                const auto xEnd{ ( tileX + 1 ) * width / tilesRoot };
                const auto yBegin{ tileY * height / tilesRoot };
                const auto yEnd{ ( tileY + 1 ) * height / tilesRoot };
                Random rnd{ 31251 + tileIndex };
                Tracer tracer{ camera, *accelerator };

                for ( int sy = 0; sy < samplesRoot; ++sy ) {
                    for ( int sx = 0; sx < samplesRoot; ++sx ) {
                        for ( int i = yBegin; i < yEnd; ++i ) {
                            for ( int j = xBegin; j < xEnd; ++j ) {
                                float px = j + ( sx + rnd.nextFloat() ) * ( 1.0f / samplesRoot );
                                float py = i + ( sy + rnd.nextFloat() ) * ( 1.0f / samplesRoot );
                                const auto hitPointIndex{ ( sy * samplesRoot + sx ) * height * width + i * width + j };

                                hitPoints[hitPointIndex] = tracer.samplePrimary( px, py );
                            }
                        }
                    }
                }
            } };
    }

    for ( auto& thread : primaryThreads ) {
        thread.join();
    }

    double primEnd = seconds();
    std::println( "{} rays/sec.",
                  std::trunc( ( width * height * samplesRoot * samplesRoot ) / ( primEnd - primStart ) ) );
    std::print( "AO rays: " );

    double aoStart = seconds();

    std::array<int, threadCount> aoRayCounts{};
    std::array<std::thread, threadCount> aoThreads{};

    auto sm = std::shared_mutex{};

    for ( int tileIndex{ 0 }; tileIndex < threadCount; ++tileIndex ) {
        aoThreads[tileIndex] = std::thread{ [&sm, &camera, accelerator, &hitPoints, &image, &aoRayCounts, width, height,
                                             samplesRoot, tilesRoot, tileIndex]() -> void {
            const auto tileX{ tileIndex % tilesRoot };
            const auto tileY{ tileIndex / tilesRoot };
            const auto xBegin{ tileX * width / tilesRoot };
            const auto xEnd{ ( tileX + 1 ) * width / tilesRoot };
            const auto yBegin{ tileY * height / tilesRoot };
            const auto yEnd{ ( tileY + 1 ) * height / tilesRoot };
            Tracer tracer{ camera, *accelerator };
            auto tileAoRays{ 0 };

            for ( int sy = 0; sy < samplesRoot; ++sy ) {
                for ( int sx = 0; sx < samplesRoot; ++sx ) {
                    for ( int i = yBegin; i < yEnd; ++i ) {
                        for ( int j = xBegin; j < xEnd; ++j ) {
                            const auto hitPointIndex{ ( sy * samplesRoot + sx ) * height * width + i * width + j };
                            Tracer::HitPoint h = hitPoints[hitPointIndex];
                            float c = 1.0f;

                            if ( h._hit ) {
                                tileAoRays += Tracer::_aoSamplesRoot * Tracer::_aoSamplesRoot;
                                c = tracer.sampleAO( h );
                            }
                            {
                                std::shared_lock _{ sm };
                                image[i * width + j] += c * ( 1.0f / ( samplesRoot * samplesRoot ) );
                            }
                        }
                    }
                }
            }

            aoRayCounts[tileIndex] = tileAoRays;
        } };
    }

    for ( auto& thread : aoThreads ) {
        thread.join();
    }

    auto aoRays{ 0 };
    for ( const auto tileAoRays : aoRayCounts ) {
        aoRays += tileAoRays;
    }

    double aoEnd = seconds();

    std::println( "{} rays/sec.", std::trunc( aoRays / ( aoEnd - aoStart ) ) );
    std::println( "Total time: {}", ( primEnd - primStart ) + ( aoEnd - aoStart ) );

    std::println( "Saving image.ppm..." );

    FILE* f = fopen( "image.ppm", "w" );
    fprintf( f, "P3\n%d %d\n%d\n", width, height, 255 );
    for ( int i = 0; i < width * height; ++i ) {
        int c = (int)( image[i] * 255.0f + 0.5f );
        fprintf( f, "%d %d %d ", c, c, c );
    }
    fclose( f );

    std::cout << "done." << std::endl;
    delete accelerator;
    return 0;
}
