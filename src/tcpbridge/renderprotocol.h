#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace tire {

constexpr size_t kRenderProtocolMaximumPayload{ 64 * 1024 * 1024 };

enum class RenderMessageType : uint8_t {
    //
    Request = 1,
    Poll = 2,
    Image = 3,
    Error = 4
};

// ======================================================================================
// ==================== RenderMessage ===================================================
// ======================================================================================

struct RenderMessage final {
    RenderMessage( const std::string &jsonString );

private:
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
