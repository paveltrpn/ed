#include "tcpbridge/framecodec.h"

#include <cstdint>
#include <limits>
#include <stdexcept>

namespace tire {

FrameCodec::FrameCodec( size_t maximumPayload )
    : _maximumPayload{ maximumPayload } {}

auto FrameCodec::push( const std::byte *data, size_t size ) -> FrameDecodeResult {
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
            result._status = FrameStatus::Oversized;
            return result;
        }

        const auto available{ _buffer.size() - offset - _prefixSize };
        if ( payloadSize > available ) {
            break;
        }

        const auto *payloadBegin{ _buffer.data() + offset + _prefixSize };
        result._messages.emplace_back( payloadBegin, payloadBegin + payloadSize );
        offset += _prefixSize + payloadSize;
    }

    if ( offset != 0 ) {
        _buffer.erase( _buffer.cbegin(), _buffer.cbegin() + static_cast<std::ptrdiff_t>( offset ) );
    }

    return result;
}

auto FrameCodec::push( const Bytes &data ) -> FrameDecodeResult {
    return push( data.data(), data.size() );
}

auto FrameCodec::endOfStream() const noexcept -> FrameStatus {
    if ( _buffer.empty() ) {
        return FrameStatus::Ok;
    }

    return FrameStatus::PartialEof;
}

auto FrameCodec::reset() -> void {
    _buffer.clear();
}

auto FrameCodec::encode( const Bytes &payload ) -> Bytes {
    if ( payload.size() > std::numeric_limits<uint32_t>::max() ) {
        throw std::length_error{ "Frame payload exceeds 32-bit length prefix" };
    }

    const auto payloadSize{ static_cast<uint32_t>( payload.size() ) };
    Bytes encoded{ static_cast<std::byte>( ( payloadSize >> 24U ) & 0xffU ),
                   static_cast<std::byte>( ( payloadSize >> 16U ) & 0xffU ),
                   static_cast<std::byte>( ( payloadSize >> 8U ) & 0xffU ),
                   static_cast<std::byte>( payloadSize & 0xffU ) };
    encoded.insert( encoded.cend(), payload.cbegin(), payload.cend() );

    return encoded;
}

}
