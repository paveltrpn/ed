#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "tcpbridge/renderprotocol.h"

namespace tire {

enum class RenderJobState {
    Starting,
    Rendering,
    Complete,
    Failed
};

// ======================================================================================
// ==================== RenderJobSnapshot ===============================================
// ======================================================================================

struct RenderJobSnapshot {
    RenderJobState _state{ RenderJobState::Starting };
    std::string _error{};
    std::vector<float> _pixels{};
};

// ======================================================================================
// ==================== RenderJob =======================================================
// ======================================================================================

struct RenderJob final {
    RenderJob( uint32_t width, uint32_t height, std::string sceneObj, RenderCamera camera );
    ~RenderJob();

    RenderJob( const RenderJob & ) = delete;
    RenderJob( RenderJob && ) = delete;
    auto operator=( const RenderJob & ) -> RenderJob & = delete;
    auto operator=( RenderJob && ) -> RenderJob & = delete;

    auto start() -> void;
    auto cancel() -> void;

    [[nodiscard]] auto width() const noexcept -> uint32_t;
    [[nodiscard]] auto height() const noexcept -> uint32_t;
    [[nodiscard]] auto snapshot() const -> RenderJobSnapshot;

private:
    auto run() -> void;
    auto setState( RenderJobState state ) -> void;
    auto fail( const std::string &message ) -> void;
    auto publishTile( size_t xBegin, size_t xEnd, size_t yBegin, size_t yEnd,
                      const std::vector<float> &tilePixels ) -> void;

    static auto joinThreads( std::vector<std::thread> &threads ) -> void;

private:
    uint32_t _width{ 0 };
    uint32_t _height{ 0 };
    std::string _sceneObj{};
    RenderCamera _camera{};
    std::vector<float> _image{};
    mutable std::mutex _snapshotMutex{};
    RenderJobState _state{ RenderJobState::Starting };
    std::string _error{};
    std::atomic<bool> _cancellation{ false };
    std::thread _worker{};
};

}  // namespace tire
