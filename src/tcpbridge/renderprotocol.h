#pragma once

#include <cstring>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <chrono>

#include <nlohmann/json.hpp>

namespace tire {

constexpr size_t kRenderProtocolMaximumPayload{ 64 * 1024 * 1024 };

enum class RenderMessageType : uint8_t {
    //
    UNDEFINED = 0,
    REQUEST = 1,
    POLL = 2,
    IMAGE = 3,
    ERROR = 4,
    CHAT = 5
};

// ======================================================================================
// ==================== RenderMessage ===================================================
// ======================================================================================

struct RenderMessage {
    auto encode() const -> std::vector<std::byte>;

    virtual auto toJson() const -> std::string = 0;
    virtual auto parse( std::string jsonString ) -> void;

    auto type() const -> RenderMessageType;
    auto setType( RenderMessageType value ) const -> void;

private:
    auto messageFromString( const std::string &str ) const -> std::vector<std::byte> {
        auto message = std::vector<std::byte>{};

        message.resize( static_cast<size_t>( str.size() ) );
        std::memcpy( message.data(), str.data(), static_cast<size_t>( str.size() ) );

        return message;
    }

protected:
    const uint32_t _magic{ 0xffaabbcc };
    RenderMessageType _type{ RenderMessageType::UNDEFINED };
    std::chrono::time_point<std::chrono::system_clock> _timestamp{};
};

// ======================================================================================
// ==================== ChatMessage =====================================================
// ======================================================================================

struct ChatMessage : RenderMessage {
    ChatMessage( const std::string &msg );

    auto toJson() const -> std::string override;
    auto parse( std::string jsonString ) -> void override;

private:
    std::string _msg{};
};

// ======================================================================================
// ==================== RequestRenderMessage ============================================
// ======================================================================================

struct RequestRenderMessage : RenderMessage {
    RequestRenderMessage( uint32_t width, uint32_t height, std::string accelerator, uint32_t threadCount );

    auto toJson() const -> std::string override;
    auto parse( std::string jsonString ) -> void override;

private:
    uint32_t _width{ 0 };
    uint32_t _height{ 0 };
    std::string _accelerator;
    uint32_t _threadCount;
};

// ======================================================================================
// ==================== RenderRequestPayload ============================================
// ======================================================================================

struct RenderRequestPayload {
    uint32_t _width{ 0 };
    uint32_t _height{ 0 };
};

// ======================================================================================
// ==================== RenderImagePayload ==============================================
// ======================================================================================

struct RenderImagePayload {
    uint32_t _width{ 0 };
    uint32_t _height{ 0 };
    bool _final{ false };
    std::vector<std::byte> _pixels{};
};

auto renderMessageType( const std::vector<std::byte> &payload ) -> RenderMessageType;
auto renderPixelCount( uint32_t width, uint32_t height ) -> uint32_t;
auto renderImageSizeIsValid( uint32_t width, uint32_t height ) noexcept -> bool;

auto encodeRenderRequest( uint32_t width, uint32_t height ) -> std::vector<std::byte>;
auto decodeRenderRequest( const std::vector<std::byte> &payload ) -> RenderRequestPayload;

auto encodeRenderPollRequest() -> std::vector<std::byte>;
auto decodeRenderPollRequest( const std::vector<std::byte> &payload ) -> void;

auto encodeRenderImage( const RenderImagePayload &image ) -> std::vector<std::byte>;
auto decodeRenderImage( const std::vector<std::byte> &payload ) -> RenderImagePayload;

auto encodeRenderError( std::string_view text ) -> std::vector<std::byte>;
auto decodeRenderError( const std::vector<std::byte> &payload ) -> std::string;

}  // namespace tire
