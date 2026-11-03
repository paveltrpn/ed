#include "tcpbridge/renderprotocol.h"

#include <cstring>
#include <limits>
#include <stdexcept>
#include <utility>

namespace tire {

namespace {

constexpr size_t kTypeSize{ 1 };
constexpr size_t kRequestPayloadSize{ 9 };
constexpr size_t kImageHeaderSize{ 14 };

auto appendU32( std::vector<std::byte> &payload, uint32_t value ) -> void {
    payload.push_back( static_cast<std::byte>( ( value >> 24U ) & 0xffU ) );
    payload.push_back( static_cast<std::byte>( ( value >> 16U ) & 0xffU ) );
    payload.push_back( static_cast<std::byte>( ( value >> 8U ) & 0xffU ) );
    payload.push_back( static_cast<std::byte>( value & 0xffU ) );
}

auto readU32( const std::vector<std::byte> &payload, size_t offset ) -> uint32_t {
    return ( std::to_integer<uint32_t>( payload[offset] ) << 24U ) |
           ( std::to_integer<uint32_t>( payload[offset + 1] ) << 16U ) |
           ( std::to_integer<uint32_t>( payload[offset + 2] ) << 8U ) |
           std::to_integer<uint32_t>( payload[offset + 3] );
}

auto requireType( const std::vector<std::byte> &payload, RenderMessageType expected ) -> void {
    if ( renderMessageType( payload ) != expected ) {
        throw std::invalid_argument{ "Unexpected render protocol message type" };
    }
}

}  // namespace

auto renderMessageType( const std::vector<std::byte> &payload ) -> RenderMessageType {
    if ( payload.empty() ) {
        throw std::invalid_argument{ "Empty render protocol message" };
    }

    const auto type = static_cast<RenderMessageType>( std::to_integer<uint8_t>( payload.front() ) );
    switch ( type ) {
        case RenderMessageType::REQUEST:
        case RenderMessageType::POLL:
        case RenderMessageType::IMAGE:
        case RenderMessageType::ERROR:
            return type;
    }

    throw std::invalid_argument{ "Unknown render protocol message type" };
}

auto renderPixelCount( uint32_t width, uint32_t height ) -> uint32_t {
    if ( width == 0 || height == 0 ) {
        throw std::invalid_argument{ "Render dimensions must be positive" };
    }

    const auto wideWidth = static_cast<size_t>( width );
    const auto wideHeight = static_cast<size_t>( height );
    if ( wideHeight > std::numeric_limits<size_t>::max() / wideWidth ) {
        throw std::overflow_error{ "Render dimensions overflow the pixel count" };
    }

    const auto pixelCount = wideWidth * wideHeight;
    if ( pixelCount > std::numeric_limits<uint32_t>::max() ) {
        throw std::overflow_error{ "Render dimensions exceed the 32-bit pixel count" };
    }

    if ( pixelCount > kRenderProtocolMaximumPayload - kImageHeaderSize ) {
        throw std::length_error{ "Render image exceeds the protocol frame limit" };
    }

    return static_cast<uint32_t>( pixelCount );
}

auto renderImageSizeIsValid( uint32_t width, uint32_t height ) noexcept -> bool {
    try {
        renderPixelCount( width, height );
        return true;
    } catch ( ... ) {
        return false;
    }
}

auto encodeRenderRequest( uint32_t width, uint32_t height ) -> std::vector<std::byte> {
    renderPixelCount( width, height );

    auto payload = std::vector<std::byte>{};
    payload.reserve( kRequestPayloadSize );
    payload.push_back( static_cast<std::byte>( RenderMessageType::REQUEST ) );
    appendU32( payload, width );
    appendU32( payload, height );
    return payload;
}

auto decodeRenderRequest( const std::vector<std::byte> &payload ) -> RenderRequestPayload {
    requireType( payload, RenderMessageType::REQUEST );
    if ( payload.size() != kRequestPayloadSize ) {
        throw std::invalid_argument{ "Render request has an invalid length" };
    }

    const auto width = readU32( payload, kTypeSize );
    const auto height = readU32( payload, kTypeSize + sizeof( uint32_t ) );
    renderPixelCount( width, height );
    return RenderRequestPayload{ width, height };
}

auto encodeRenderPollRequest() -> std::vector<std::byte> {
    return std::vector<std::byte>{ static_cast<std::byte>( RenderMessageType::POLL ) };
}

auto decodeRenderPollRequest( const std::vector<std::byte> &payload ) -> void {
    requireType( payload, RenderMessageType::POLL );
    if ( payload.size() != kTypeSize ) {
        throw std::invalid_argument{ "Render poll has an invalid length" };
    }
}

auto encodeRenderImage( const RenderImagePayload &image ) -> std::vector<std::byte> {
    const auto pixelCount = renderPixelCount( image._width, image._height );
    if ( image._pixels.size() != pixelCount ) {
        throw std::invalid_argument{ "Render image pixel count does not match its dimensions" };
    }

    auto payload = std::vector<std::byte>{};
    payload.reserve( kImageHeaderSize + image._pixels.size() );
    payload.push_back( static_cast<std::byte>( RenderMessageType::IMAGE ) );
    appendU32( payload, image._width );
    appendU32( payload, image._height );
    auto finalValue = uint8_t{ 0 };
    if ( image._final ) {
        finalValue = 1;
    }
    payload.push_back( static_cast<std::byte>( finalValue ) );
    appendU32( payload, pixelCount );
    payload.insert( payload.cend(), image._pixels.cbegin(), image._pixels.cend() );
    return payload;
}

auto decodeRenderImage( const std::vector<std::byte> &payload ) -> RenderImagePayload {
    requireType( payload, RenderMessageType::IMAGE );
    if ( payload.size() < kImageHeaderSize ) {
        throw std::invalid_argument{ "Render image response is truncated" };
    }

    const auto width = readU32( payload, kTypeSize );
    const auto height = readU32( payload, kTypeSize + sizeof( uint32_t ) );
    const auto finalValue = std::to_integer<uint8_t>( payload[9] );
    if ( finalValue > 1 ) {
        throw std::invalid_argument{ "Render image response has an invalid final flag" };
    }

    const auto expectedPixelCount = renderPixelCount( width, height );
    const auto pixelCount = readU32( payload, 10 );
    if ( pixelCount != expectedPixelCount ) {
        throw std::invalid_argument{ "Render image response has an inconsistent pixel count" };
    }

    if ( payload.size() != kImageHeaderSize + static_cast<size_t>( pixelCount ) ) {
        throw std::invalid_argument{ "Render image response has an invalid length" };
    }

    auto pixels =
        std::vector<std::byte>{ payload.cbegin() + static_cast<std::ptrdiff_t>( kImageHeaderSize ), payload.cend() };
    return RenderImagePayload{ width, height, finalValue == 1, std::move( pixels ) };
}

auto encodeRenderError( std::string_view text ) -> std::vector<std::byte> {
    if ( text.size() > kRenderProtocolMaximumPayload - kTypeSize ) {
        throw std::length_error{ "Render error exceeds the protocol frame limit" };
    }

    auto payload = std::vector<std::byte>( kTypeSize + text.size() );
    payload.front() = static_cast<std::byte>( RenderMessageType::ERROR );
    if ( !text.empty() ) {
        std::memcpy( payload.data() + kTypeSize, text.data(), text.size() );
    }
    return payload;
}

auto decodeRenderError( const std::vector<std::byte> &payload ) -> std::string {
    requireType( payload, RenderMessageType::ERROR );
    if ( payload.size() > kRenderProtocolMaximumPayload ) {
        throw std::length_error{ "Render error exceeds the protocol frame limit" };
    }

    auto text = std::string( payload.size() - kTypeSize, '\0' );
    if ( !text.empty() ) {
        std::memcpy( text.data(), payload.data() + kTypeSize, text.size() );
    }
    return text;
}

// ======================================================================================
// ==================== RenderMessage ===================================================
// ======================================================================================

}  // namespace tire
