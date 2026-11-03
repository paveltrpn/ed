#pragma once

#include <array>
#include <cstring>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
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
    RenderMessage() = default;
    virtual ~RenderMessage() = default;

    RenderMessage( const RenderMessage& ) = default;
    RenderMessage( RenderMessage&& ) = default;

    // NOTE: explicitely delete, because pf const field.
    auto operator=( const RenderMessage& ) -> RenderMessage& = delete;
    auto operator=( RenderMessage&& ) -> RenderMessage& = delete;

    auto encode() const -> std::vector<std::byte>;

    virtual auto toJson() const -> std::string = 0;
    virtual auto parse( std::string jsonString ) -> void;

    auto type() const -> RenderMessageType;

    static auto create( const std::vector<std::byte>& payload ) -> std::unique_ptr<RenderMessage>;

protected:
    auto writeHeader( nlohmann::json& root ) const -> void;
    auto readHeader( const nlohmann::json& root ) -> void;

    static auto requirePayload( const nlohmann::json& root ) -> const nlohmann::json&;

    static auto readUint32( const nlohmann::json& object, std::string_view name ) -> uint32_t;
    static auto readBool( const nlohmann::json& object, std::string_view name ) -> bool;
    static auto readString( const nlohmann::json& object, std::string_view name ) -> std::string;

    static auto parseJson( const std::string& jsonString ) -> nlohmann::json;

private:
    auto messageFromString( const std::string& str ) const -> std::vector<std::byte>;

    static auto stringFromMessage( const std::vector<std::byte>& message ) -> std::string;

    static auto fieldError( std::string_view prefix, std::string_view name, std::string_view suffix )
        -> std::runtime_error;

protected:
    const uint32_t _magic{ 0xffaabbcc };
    RenderMessageType _type{ RenderMessageType::UNDEFINED };
    std::chrono::time_point<std::chrono::system_clock> _timestamp{};
};

// ======================================================================================
// ==================== ChatMessage =====================================================
// ======================================================================================

struct ChatMessage final : RenderMessage {
    ChatMessage() = default;
    ChatMessage( RenderMessageType type, const std::string& msg );
    explicit ChatMessage( const std::string& msg );

    auto toJson() const -> std::string override;
    auto parse( std::string jsonString ) -> void override;

    auto message() const -> std::string;

private:
    std::string _msg{};
};

// ======================================================================================
// ==================== ImageMessage ====================================================
// ======================================================================================

struct ImageMessage final : RenderMessage {
    ImageMessage() = default;
    ImageMessage( uint32_t width, uint32_t height, bool isFinal, const std::vector<std::byte>& pixels );

    auto toJson() const -> std::string override;
    auto parse( std::string jsonString ) -> void override;

    auto width() const -> uint32_t;
    auto height() const -> uint32_t;
    auto final() const -> bool;

    // Decode base64 encoded string to raw bytes.
    auto pixels() const -> std::vector<std::byte>;

    static auto pixelCount( uint32_t width, uint32_t height ) -> uint32_t;
    static auto sizeIsValid( uint32_t width, uint32_t height ) noexcept -> bool;

private:
    static constexpr size_t _imageJsonReserve{ 4096 };

private:
    uint32_t _width{ 0 };
    uint32_t _height{ 0 };

    bool _final{ false };

    // NOTE: Base64 encoded.
    std::string _pixels{};
};

// ======================================================================================
// ==================== RenderCamera ====================================================
// ======================================================================================

struct RenderCamera {
    std::array<double, 3> _eye{};
    std::array<double, 3> _target{};
    std::array<double, 3> _up{};
    double _fieldOfViewY{ 0.0 };
    double _nearDistance{ 0.0 };
    double _farDistance{ 0.0 };
};

// ======================================================================================
// ==================== RequestRenderMessage ============================================
// ======================================================================================

struct RequestRenderMessage final : RenderMessage {
    RequestRenderMessage() = default;
    RequestRenderMessage( uint32_t width, uint32_t height, std::string sceneObj, RenderCamera camera,
                          std::string accelerator = "sparse", uint32_t threadCount = 4 );

    auto toJson() const -> std::string override;
    auto parse( std::string jsonString ) -> void override;

    auto width() const -> uint32_t;
    auto height() const -> uint32_t;
    auto sceneObj() const -> std::string;
    auto camera() const -> RenderCamera;
    auto accelerator() const -> std::string;
    auto threadCount() const -> uint32_t;

private:
    static auto readDouble( const nlohmann::json& object, std::string_view name ) -> double;
    static auto readVector( const nlohmann::json& object, std::string_view name ) -> std::array<double, 3>;
    static auto readCamera( const nlohmann::json& payload ) -> RenderCamera;
    static auto validateSceneAndCamera( const std::string& sceneObj, const RenderCamera& camera ) -> void;

private:
    uint32_t _width{ 0 };
    uint32_t _height{ 0 };
    std::string _sceneObj{};
    RenderCamera _camera{};
    std::string _accelerator{};
    uint32_t _threadCount{ 0 };
};

}  // namespace tire
