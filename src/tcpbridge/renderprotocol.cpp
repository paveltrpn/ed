#include "tcpbridge/renderprotocol.h"

#include <algorithm>
#include <cstring>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>

#include <base64.hpp>

namespace tire {

// ======================================================================================
// ==================== RenderMessage ===================================================
// ======================================================================================

auto RenderMessage::encode() const -> std::vector<std::byte> {
    const auto thisJsonString = this->toJson();
    return messageFromString( thisJsonString );
}

auto RenderMessage::parse( std::string jsonString ) -> void {
    const auto root = parseJson( jsonString );
    readHeader( root );
}

auto RenderMessage::type() const -> RenderMessageType {
    return _type;
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

auto RenderMessage::parseJson( const std::string &jsonString ) -> nlohmann::json {
    try {
        return nlohmann::json::parse( jsonString );
    } catch ( const nlohmann::json::exception &exception ) {
        throw std::runtime_error{ std::string{ "Failed to parse render protocol JSON: " } + exception.what() };
    }
}

auto RenderMessage::messageFromString( const std::string &str ) const -> std::vector<std::byte> {
    auto message = std::vector<std::byte>{};

    message.resize( static_cast<size_t>( str.size() ) );
    std::memcpy( message.data(), str.data(), static_cast<size_t>( str.size() ) );

    return message;
}

auto RenderMessage::stringFromMessage( const std::vector<std::byte> &message ) -> std::string {
    auto str = std::string( message.size(), '\0' );
    if ( !message.empty() ) {
        std::memcpy( str.data(), message.data(), message.size() );
    }
    return str;
}

auto RenderMessage::fieldError( std::string_view prefix, std::string_view name, std::string_view suffix )
    -> std::runtime_error {
    return std::runtime_error{ std::string{ prefix } + std::string{ name } + std::string{ suffix } };
}

// ======================================================================================
// ==================== ChatMessage =====================================================
// ======================================================================================

ChatMessage::ChatMessage( RenderMessageType type, const std::string &msg ) {
    if ( type != RenderMessageType::CHAT && type != RenderMessageType::POLL && type != RenderMessageType::ERROR ) {
        throw std::runtime_error{ "ChatMessage supports only CHAT, POLL and ERROR message types" };
    }

    _type = type;
    _timestamp = std::chrono::system_clock::now();
    _msg = msg;
}

ChatMessage::ChatMessage( const std::string &msg )
    : ChatMessage{ RenderMessageType::CHAT, msg } {
}

auto ChatMessage::message() const -> std::string {
    return _msg;
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
    const auto root = parseJson( jsonString );
    readHeader( root );

    if ( _type != RenderMessageType::CHAT && _type != RenderMessageType::POLL && _type != RenderMessageType::ERROR ) {
        throw std::runtime_error{ "Render protocol message type does not match ChatMessage" };
    }

    const auto &payload = requirePayload( root );
    _msg = readString( payload, "message" );
}

// ======================================================================================
// ==================== RequestRenderMessage ============================================
// ======================================================================================

RequestRenderMessage::RequestRenderMessage( uint32_t width, uint32_t height, std::string accelerator,
                                            uint32_t threadCount ) {
    ImageMessage::pixelCount( width, height );

    _type = RenderMessageType::REQUEST;
    _timestamp = std::chrono::system_clock::now();
    _width = width;
    _height = height;
    _accelerator = std::move( accelerator );
    _threadCount = threadCount;
}

auto RequestRenderMessage::width() const -> uint32_t {
    return _width;
}

auto RequestRenderMessage::height() const -> uint32_t {
    return _height;
}

auto RequestRenderMessage::accelerator() const -> std::string {
    return _accelerator;
}

auto RequestRenderMessage::threadCount() const -> uint32_t {
    return _threadCount;
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
    const auto root = parseJson( jsonString );
    readHeader( root );

    if ( _type != RenderMessageType::REQUEST ) {
        throw std::runtime_error{ "Render protocol message type does not match RequestRenderMessage" };
    }

    const auto &payload = requirePayload( root );
    _width = readUint32( payload, "width" );
    _height = readUint32( payload, "height" );
    _accelerator = readString( payload, "accelerator" );
    _threadCount = readUint32( payload, "threadCount" );
}

// ======================================================================================
// ==================== ImageMessage ====================================================
// ======================================================================================

ImageMessage::ImageMessage( uint32_t width, uint32_t height, bool isFinal, const std::vector<std::byte> &pixels ) {
    const auto pixelCount{ ImageMessage::pixelCount( width, height ) };
    if ( pixels.size() != static_cast<size_t>( pixelCount ) ) {
        throw std::runtime_error{ "Render image pixel count does not match its dimensions" };
    }

    _type = RenderMessageType::IMAGE;
    _timestamp = std::chrono::system_clock::now();
    _width = width;
    _height = height;
    _final = isFinal;

    // NOTE: Encoded from an unsigned char buffer: the base64 library fill-initializes its
    // output buffer from a char literal, which does not implicitly convert to std::byte.
    auto rawPixels = std::vector<unsigned char>( pixels.size() );
    std::transform( pixels.cbegin(), pixels.cend(), rawPixels.begin(),
                    []( std::byte value ) -> unsigned char { return std::to_integer<unsigned char>( value ); } );

    _pixels = base64::encode_into<std::string>( rawPixels.cbegin(), rawPixels.cend() );
}

auto ImageMessage::width() const -> uint32_t {
    return _width;
}

auto ImageMessage::height() const -> uint32_t {
    return _height;
}

auto ImageMessage::final() const -> bool {
    return _final;
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
    const auto root = parseJson( jsonString );
    readHeader( root );

    if ( _type != RenderMessageType::IMAGE ) {
        throw std::runtime_error{ "Render protocol message type does not match ImageMessage" };
    }

    const auto &payload = requirePayload( root );
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

auto ImageMessage::pixelCount( uint32_t width, uint32_t height ) -> uint32_t {
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

    if ( pixelCount + pixelCount / 3 + _imageJsonReserve > kRenderProtocolMaximumPayload ) {
        throw std::length_error{ "Render image exceeds the protocol frame limit" };
    }

    return static_cast<uint32_t>( pixelCount );
}

auto ImageMessage::sizeIsValid( uint32_t width, uint32_t height ) noexcept -> bool {
    try {
        pixelCount( width, height );
        return true;
    } catch ( ... ) {
        return false;
    }
}

// ======================================================================================
// ==================== RenderMessage factory ===========================================
// ======================================================================================

auto RenderMessage::create( const std::vector<std::byte> &payload ) -> std::unique_ptr<RenderMessage> {
    const auto jsonString{ stringFromMessage( payload ) };
    const auto root = parseJson( jsonString );

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
        case static_cast<int64_t>( RenderMessageType::CHAT ):
        case static_cast<int64_t>( RenderMessageType::POLL ):
        case static_cast<int64_t>( RenderMessageType::ERROR ): {
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
