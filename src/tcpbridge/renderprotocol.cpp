#include "tcpbridge/renderprotocol.h"

#include <algorithm>
#include <cstring>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>

#include <base64.hpp>

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

namespace {

auto parseJsonString( const std::string &jsonString ) -> nlohmann::json {
    try {
        return nlohmann::json::parse( jsonString );
    } catch ( const nlohmann::json::exception &exception ) {
        throw std::runtime_error{ std::string{ "Failed to parse render protocol JSON: " } + exception.what() };
    }
}

auto stringFromMessage( const std::vector<std::byte> &message ) -> std::string {
    auto str = std::string( message.size(), '\0' );
    if ( !message.empty() ) {
        std::memcpy( str.data(), message.data(), message.size() );
    }
    return str;
}

auto fieldError( std::string_view prefix, std::string_view name, std::string_view suffix ) -> std::runtime_error {
    return std::runtime_error{ std::string{ prefix } + std::string{ name } + std::string{ suffix } };
}

}  // namespace

auto RenderMessage::encode() const -> std::vector<std::byte> {
    const auto thisJsonString = this->toJson();
    return messageFromString( thisJsonString );
}

auto RenderMessage::parse( std::string jsonString ) -> void {
    const auto root{ parseJsonString( jsonString ) };
    readHeader( root );
}

auto RenderMessage::type() const -> RenderMessageType {
    return _type;
}

auto RenderMessage::setType( RenderMessageType value ) -> void {
    _type = value;
}

auto RenderMessage::writeHeader( nlohmann::json &root ) const -> void {
    root["magic"] = _magic;
    root["type"] = static_cast<int>( _type );
    root["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>( _timestamp.time_since_epoch() ).count();
}

auto RenderMessage::readHeader( const nlohmann::json &root ) -> void {
    if ( !root.contains( "magic" ) ) {
        throw std::runtime_error{ "Render protocol message is missing the magic field" };
    }

    const auto &magic = root["magic"];
    if ( !magic.is_number_unsigned() || magic.get<uint64_t>() != static_cast<uint64_t>( _magic ) ) {
        throw std::runtime_error{ "Render protocol message has an invalid magic value" };
    }

    if ( !root.contains( "type" ) ) {
        throw std::runtime_error{ "Render protocol message is missing the type field" };
    }

    const auto &type = root["type"];
    if ( !type.is_number_integer() ) {
        throw std::runtime_error{ "Render protocol message has a non-integer type field" };
    }

    const auto rawType = type.get<int64_t>();
    if ( rawType < static_cast<int64_t>( RenderMessageType::REQUEST ) ||
         rawType > static_cast<int64_t>( RenderMessageType::CHAT ) ) {
        throw std::runtime_error{ "Render protocol message has an out of range type value" };
    }

    _type = static_cast<RenderMessageType>( rawType );

    if ( !root.contains( "timestamp" ) ) {
        throw std::runtime_error{ "Render protocol message is missing the timestamp field" };
    }

    const auto &timestamp = root["timestamp"];
    if ( !timestamp.is_number_integer() ) {
        throw std::runtime_error{ "Render protocol message has a non-integer timestamp field" };
    }

    _timestamp = std::chrono::system_clock::time_point{ std::chrono::milliseconds{ timestamp.get<int64_t>() } };
}

auto RenderMessage::requirePayload( const nlohmann::json &root ) -> const nlohmann::json & {
    if ( !root.contains( "payload" ) ) {
        throw std::runtime_error{ "Render protocol message is missing the payload field" };
    }

    const auto &payload = root["payload"];
    if ( !payload.is_object() ) {
        throw std::runtime_error{ "Render protocol message payload is not an object" };
    }

    return payload;
}

auto RenderMessage::readUint32( const nlohmann::json &object, std::string_view name ) -> uint32_t {
    if ( !object.contains( name ) ) {
        throw fieldError( "Render protocol payload is missing the ", name, " field" );
    }

    const auto &value = object[name];
    if ( !value.is_number_integer() ) {
        throw fieldError( "Render protocol payload field ", name, " is not an integer" );
    }

    const auto rawValue = value.get<int64_t>();
    if ( rawValue < 0 || rawValue > static_cast<int64_t>( std::numeric_limits<uint32_t>::max() ) ) {
        throw fieldError( "Render protocol payload field ", name, " is out of the unsigned 32-bit range" );
    }

    return static_cast<uint32_t>( rawValue );
}

auto RenderMessage::readBool( const nlohmann::json &object, std::string_view name ) -> bool {
    if ( !object.contains( name ) ) {
        throw fieldError( "Render protocol payload is missing the ", name, " field" );
    }

    const auto &value = object[name];
    if ( !value.is_boolean() ) {
        throw fieldError( "Render protocol payload field ", name, " is not a boolean" );
    }

    return value.get<bool>();
}

auto RenderMessage::readString( const nlohmann::json &object, std::string_view name ) -> std::string {
    if ( !object.contains( name ) ) {
        throw fieldError( "Render protocol payload is missing the ", name, " field" );
    }

    const auto &value = object[name];
    if ( !value.is_string() ) {
        throw fieldError( "Render protocol payload field ", name, " is not a string" );
    }

    return value.get<std::string>();
}

// ======================================================================================
// ==================== ChatMessage =====================================================
// ======================================================================================

ChatMessage::ChatMessage( const std::string &msg ) {
    _type = RenderMessageType::CHAT;
    _timestamp = std::chrono::system_clock::now();
    _msg = msg;
}

auto ChatMessage::toJson() const -> std::string {
    auto root = nlohmann::json::object();
    writeHeader( root );

    auto payload = nlohmann::json::object();
    payload["message"] = _msg;
    root["payload"] = std::move( payload );

    return root.dump();
}

auto ChatMessage::parse( std::string jsonString ) -> void {
    const auto root{ parseJsonString( jsonString ) };
    readHeader( root );

    if ( _type != RenderMessageType::CHAT ) {
        throw std::runtime_error{ "Render protocol message type does not match ChatMessage" };
    }

    const auto &payload{ requirePayload( root ) };
    _msg = readString( payload, "message" );
}

// ======================================================================================
// ==================== RequestRenderMessage ============================================
// ======================================================================================

RequestRenderMessage::RequestRenderMessage( uint32_t width, uint32_t height, std::string accelerator,
                                            uint32_t threadCount ) {
    _type = RenderMessageType::REQUEST;
    _timestamp = std::chrono::system_clock::now();
    _width = width;
    _height = height;
    _accelerator = std::move( accelerator );
    _threadCount = threadCount;
}

auto RequestRenderMessage::toJson() const -> std::string {
    auto root = nlohmann::json::object();
    writeHeader( root );

    auto payload = nlohmann::json::object();
    payload["width"] = _width;
    payload["height"] = _height;
    payload["accelerator"] = _accelerator;
    payload["threadCount"] = _threadCount;
    root["payload"] = std::move( payload );

    return root.dump();
}

auto RequestRenderMessage::parse( std::string jsonString ) -> void {
    const auto root{ parseJsonString( jsonString ) };
    readHeader( root );

    if ( _type != RenderMessageType::REQUEST ) {
        throw std::runtime_error{ "Render protocol message type does not match RequestRenderMessage" };
    }

    const auto &payload{ requirePayload( root ) };
    _width = readUint32( payload, "width" );
    _height = readUint32( payload, "height" );
    _accelerator = readString( payload, "accelerator" );
    _threadCount = readUint32( payload, "threadCount" );
}

// ======================================================================================
// ==================== ImageMessage ====================================================
// ======================================================================================

ImageMessage::ImageMessage( uint32_t width, uint32_t height ) {
    _type = RenderMessageType::IMAGE;
    _timestamp = std::chrono::system_clock::now();
    _width = width;
    _height = height;
}

auto ImageMessage::toJson() const -> std::string {
    auto root = nlohmann::json::object();
    writeHeader( root );

    auto payload = nlohmann::json::object();
    payload["width"] = _width;
    payload["height"] = _height;
    payload["final"] = _final;
    payload["pixels"] = _pixels;
    root["payload"] = std::move( payload );

    return root.dump();
}

auto ImageMessage::parse( std::string jsonString ) -> void {
    const auto root{ parseJsonString( jsonString ) };
    readHeader( root );

    if ( _type != RenderMessageType::IMAGE ) {
        throw std::runtime_error{ "Render protocol message type does not match ImageMessage" };
    }

    const auto &payload{ requirePayload( root ) };
    _width = readUint32( payload, "width" );
    _height = readUint32( payload, "height" );
    _final = readBool( payload, "final" );
    _pixels = readString( payload, "pixels" );

    const auto pixels{ this->pixels() };
    const auto expectedPixelCount = static_cast<size_t>( _width ) * static_cast<size_t>( _height );

    if ( pixels.size() != expectedPixelCount ) {
        throw std::runtime_error{ "Render image pixel count does not match its dimensions" };
    }
}

auto ImageMessage::pixels() const -> std::vector<std::byte> {
    // NOTE: Decoded into unsigned char buffer: the base64 library fill-initializes its
    // output buffer from a char literal, which does not implicitly convert to std::byte.
    const auto rawPixels{ base64::decode_into<std::vector<unsigned char>>( _pixels ) };

    auto pixels = std::vector<std::byte>( rawPixels.size() );
    std::transform( rawPixels.cbegin(), rawPixels.cend(), pixels.begin(),
                    []( unsigned char value ) -> std::byte { return static_cast<std::byte>( value ); } );

    return pixels;
}

// ======================================================================================
// ==================== Render message factory ==========================================
// ======================================================================================

auto createRenderMessage( const std::vector<std::byte> &payload ) -> std::unique_ptr<RenderMessage> {
    const auto jsonString{ stringFromMessage( payload ) };
    const auto root{ parseJsonString( jsonString ) };

    if ( !root.is_object() ) {
        throw std::runtime_error{ "Render protocol message is not a JSON object" };
    }

    if ( !root.contains( "type" ) ) {
        throw std::runtime_error{ "Render protocol message is missing the type field" };
    }

    const auto &type = root["type"];

    if ( !type.is_number_integer() ) {
        throw std::runtime_error{ "Render protocol message has a non-integer type field" };
    }

    const auto rawType = type.get<int64_t>();
    auto message = std::unique_ptr<RenderMessage>{};

    switch ( rawType ) {
        case static_cast<int64_t>( RenderMessageType::REQUEST ): {
            message = std::make_unique<RequestRenderMessage>();
            break;
        }
        case static_cast<int64_t>( RenderMessageType::IMAGE ): {
            message = std::make_unique<ImageMessage>();
            break;
        }
        case static_cast<int64_t>( RenderMessageType::CHAT ): {
            message = std::make_unique<ChatMessage>();
            break;
        }
        default: {
            throw std::runtime_error{ "Render protocol message type is not supported" };
        }
    }

    message->parse( std::move( jsonString ) );

    return message;
}

}  // namespace tire
