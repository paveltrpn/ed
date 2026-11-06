#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>

#include <uv.h>

#include "tcpbridge/tcpbridge.h"
#include "io/timer.h"

namespace tire {

// ======================================================================================
// ==================== TcpFrameCodec ===================================================
// ======================================================================================

TcpFrameCodec::TcpFrameCodec( size_t maximumPayload )
    : _maximumPayload{ maximumPayload } {
}

auto TcpFrameCodec::push( const std::byte* data, size_t size ) -> FrameDecodeResult {
    FrameDecodeResult result{};

    if ( size != 0 ) {
        _buffer.insert( _buffer.cend(), data, data + size );
    }

    size_t offset{ 0 };
    while ( _buffer.size() - offset >= _prefixSize ) {
        const auto payloadSize{ ( std::to_integer<uint32_t>( _buffer[offset] ) << 24U ) |
                                ( std::to_integer<uint32_t>( _buffer[offset + 1] ) << 16U ) |
                                ( std::to_integer<uint32_t>( _buffer[offset + 2] ) << 8U ) |
                                std::to_integer<uint32_t>( _buffer[offset + 3] ) };

        if ( payloadSize > _maximumPayload ) {
            reset();
            result._status = FrameStatus::OVERSIZED;
            return result;
        }

        const auto available{ _buffer.size() - offset - _prefixSize };
        if ( payloadSize > available ) {
            break;
        }

        const auto* payloadBegin{ _buffer.data() + offset + _prefixSize };
        result._messages.emplace_back( payloadBegin, payloadBegin + payloadSize );
        offset += _prefixSize + payloadSize;
    }

    if ( offset != 0 ) {
        _buffer.erase( _buffer.cbegin(), _buffer.cbegin() + static_cast<std::ptrdiff_t>( offset ) );
    }

    return result;
}

auto TcpFrameCodec::push( const std::vector<std::byte>& data ) -> FrameDecodeResult {
    return push( data.data(), data.size() );
}

auto TcpFrameCodec::endOfStream() const noexcept -> FrameStatus {
    if ( _buffer.empty() ) {
        return FrameStatus::OK;
    }

    return FrameStatus::PARTIAL_EOF;
}

auto TcpFrameCodec::reset() -> void {
    _buffer.clear();
}

auto TcpFrameCodec::encode( const std::vector<std::byte>& payload ) -> std::vector<std::byte> {
    if ( payload.size() > std::numeric_limits<uint32_t>::max() ) {
        throw std::length_error{ "Frame payload exceeds 32-bit length prefix" };
    }

    const auto payloadSize{ static_cast<uint32_t>( payload.size() ) };

    std::vector<std::byte> encoded{ static_cast<std::byte>( ( payloadSize >> 24U ) & 0xffU ),
                                    static_cast<std::byte>( ( payloadSize >> 16U ) & 0xffU ),
                                    static_cast<std::byte>( ( payloadSize >> 8U ) & 0xffU ),
                                    static_cast<std::byte>( payloadSize & 0xffU ) };

    encoded.insert( encoded.cend(), payload.cbegin(), payload.cend() );

    return encoded;
}

// ======================================================================================
// ==================== TcpBridge =======================================================
// ======================================================================================

TcpBridge::TcpBridge( size_t maximumPayload )
    : _scheduler{ std::make_unique<Scheduler>() }
    , _maximumPayload{ maximumPayload } {
}

TcpBridge::~TcpBridge() {
    //
    close();
}

auto TcpBridge::listen( std::string host, uint16_t port ) -> Task<int> {
    auto listenAwaitable = _scheduler->listen( std::move( host ), port );

    auto status = 0;
    {
        std::scoped_lock lock{ _sessionMutex };

        if ( _closing ) {
            status = UV_ECANCELED;
        } else {
            _listener = listenAwaitable.listener();
        }
    }

    if ( status != 0 ) {
        co_return status;
    }

    const auto listenResult = co_await listenAwaitable;

    if ( listenResult._status != 0 ) {
        co_return listenResult._status;
    }

    auto listener = std::shared_ptr<TcpListener>{};
    {
        std::scoped_lock lock{ _sessionMutex };

        if ( !_closing ) {
            listener = _listener;
        }
    }

    if ( listener == nullptr ) {
        co_return UV_ECANCELED;
    }

    const auto acceptResult = co_await _scheduler->accept( listener );

    if ( acceptResult._status != 0 ) {
        co_return acceptResult._status;
    }

    {
        std::scoped_lock lock{ _sessionMutex };

        if ( _closing ) {
            status = UV_ECANCELED;
        } else {
            _connection = acceptResult._connection;
        }
    }

    if ( status != 0 ) {
        _scheduler->close( acceptResult._connection );
        co_return status;
    }

    co_return 0;
}

auto TcpBridge::connect( std::string host, uint16_t port, TcpBridgeRetryOptions retry ) -> Task<int> {
    if ( retry._maximumAttempts == 0 ) {
        co_return UV_EINVAL;
    }

    auto lastStatus = static_cast<int>( UV_ECANCELED );

    for ( size_t attempt = 0; attempt < retry._maximumAttempts; ++attempt ) {
        {
            std::scoped_lock lock{ _sessionMutex };

            if ( _closing ) {
                lastStatus = UV_ECANCELED;
                break;
            }
        }

        const auto connectResult = co_await _scheduler->connect( host, port );

        lastStatus = connectResult._status;

        if ( lastStatus == 0 ) {
            {
                std::scoped_lock lock{ _sessionMutex };

                if ( _closing ) {
                    lastStatus = UV_ECANCELED;
                } else {
                    _connection = connectResult._connection;
                }
            }

            if ( lastStatus != 0 ) {
                _scheduler->close( connectResult._connection );
            }

            co_return lastStatus;
        }

        if ( lastStatus == UV_ECANCELED || _closing ) {
            break;
        }

        if ( attempt + 1 < retry._maximumAttempts ) {
            {
                std::scoped_lock lock{ _sessionMutex };

                if ( _closing ) {
                    lastStatus = UV_ECANCELED;
                    break;
                }
            }

            const auto timerStatus = co_await _scheduler->timeout( retry._delayMilliseconds );

            if ( timerStatus != 0 ) {
                lastStatus = timerStatus;
                break;
            }
        }
    }

    co_return lastStatus;
}

auto TcpBridge::read() -> Task<TcpBridgeMessage> {
    enum class ReadState {
        CHECK_SESSION,
        AWAITING_DATA,
        HANDLING_READ_RESULT,
        DECODING,
        COMPLETE
    };

    auto state{ ReadState::CHECK_SESSION };
    auto connection = std::shared_ptr<TcpConnection>{};
    auto readResult = TcpReadResult{};
    auto result = TcpBridgeMessage{};

    while ( state != ReadState::COMPLETE ) {
        switch ( state ) {
            case ReadState::CHECK_SESSION: {
                {
                    std::scoped_lock lock{ _sessionMutex };

                    if ( !_pendingMessages.empty() ) {
                        result._payload = std::move( _pendingMessages.front() );
                        _pendingMessages.pop_front();
                        state = ReadState::COMPLETE;
                    } else if ( _closing ) {
                        result._status = UV_ECANCELED;
                        state = ReadState::COMPLETE;
                    } else {
                        connection = _connection;
                        state = ReadState::AWAITING_DATA;
                    }
                }

                if ( state == ReadState::AWAITING_DATA && connection == nullptr ) {
                    result._status = UV_ENOTCONN;
                    state = ReadState::COMPLETE;
                }
                break;
            }
            case ReadState::AWAITING_DATA: {
                readResult = co_await _scheduler->read( connection );
                state = ReadState::HANDLING_READ_RESULT;
                break;
            }
            case ReadState::HANDLING_READ_RESULT: {
                if ( readResult._status == 0 ) {
                    state = ReadState::DECODING;
                } else if ( readResult._status != UV_EOF ) {
                    result._status = readResult._status;
                    state = ReadState::COMPLETE;
                } else {
                    result._status = UV_EOF;
                    {
                        std::scoped_lock lock{ _sessionMutex };

                        if ( _codec.endOfStream() == FrameStatus::PARTIAL_EOF ) {
                            result._status = UV_EPROTO;
                        }
                    }
                    state = ReadState::COMPLETE;
                }
                break;
            }
            case ReadState::DECODING: {
                {
                    std::scoped_lock lock{ _sessionMutex };

                    const auto decoded = _codec.push( readResult._bytes );

                    if ( decoded._status == FrameStatus::OVERSIZED ) {
                        result._status = UV_EPROTO;
                        state = ReadState::COMPLETE;
                    } else {
                        for ( auto& decodedMessage : decoded._messages ) {
                            _pendingMessages.push_back( std::move( decodedMessage ) );
                        }

                        if ( !_pendingMessages.empty() ) {
                            result._payload = std::move( _pendingMessages.front() );
                            _pendingMessages.pop_front();
                            state = ReadState::COMPLETE;
                        } else {
                            state = ReadState::CHECK_SESSION;
                        }
                    }
                }
                break;
            }
            case ReadState::COMPLETE: {
                break;
            }
        }
    }

    co_return std::move( result );
}

auto TcpBridge::send( std::vector<std::byte> message ) -> Task<int> {
    auto connection = std::shared_ptr<TcpConnection>{};
    auto status = 0;

    {
        std::scoped_lock lock{ _sessionMutex };

        if ( _closing ) {
            status = UV_ECANCELED;
        } else {
            connection = _connection;
        }
    }

    if ( status != 0 ) {
        co_return status;
    }

    if ( connection == nullptr ) {
        co_return UV_ENOTCONN;
    }

    if ( message.size() > _maximumPayload || message.size() > std::numeric_limits<uint32_t>::max() ) {
        co_return UV_EMSGSIZE;
    }

    auto framed = TcpFrameCodec::encode( message );
    const auto writeResult = co_await _scheduler->write( connection, std::move( framed ) );

    co_return writeResult._status;
}

auto TcpBridge::delay( uint64_t timeoutMilliseconds ) -> Task<int> {
    auto status = 0;

    {
        std::scoped_lock lock{ _sessionMutex };

        if ( _closing ) {
            status = UV_ECANCELED;
        } else if ( _delayActive ) {
            status = UV_EBUSY;
        } else {
            _delayActive = true;
        }
    }

    if ( status != 0 ) {
        co_return status;
    }

    status = co_await _scheduler->timeout( timeoutMilliseconds );

    {
        std::scoped_lock lock{ _sessionMutex };
        _delayActive = false;
    }

    co_return status;
}

auto TcpBridge::close() -> void {
    _closing = true;

    auto listener = std::shared_ptr<TcpListener>{};
    auto connection = std::shared_ptr<TcpConnection>{};

    {
        std::scoped_lock lock{ _sessionMutex };

        listener = _listener;
        connection = _connection;
    }

    if ( listener != nullptr ) {
        _scheduler->close( listener );
    }

    if ( connection != nullptr ) {
        _scheduler->close( connection );
    }
}

}  // namespace tire
