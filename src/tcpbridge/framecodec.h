#pragma once

#include <cstddef>
#include <vector>

namespace tire {

using Bytes = std::vector<std::byte>;

enum class FrameStatus { Ok, Oversized, PartialEof };

struct FrameDecodeResult {
    FrameStatus _status{ FrameStatus::Ok };
    std::vector<Bytes> _messages{};
};

// ======================================================================================
// ==================== FrameCodec ======================================================
// ======================================================================================

struct FrameCodec {
    static constexpr size_t _defaultMaximumPayload{ 1024 * 1024 };

    explicit FrameCodec( size_t maximumPayload = _defaultMaximumPayload );

    auto push( const std::byte *data, size_t size ) -> FrameDecodeResult;
    auto push( const Bytes &data ) -> FrameDecodeResult;
    auto endOfStream() const noexcept -> FrameStatus;
    auto reset() -> void;

    static auto encode( const Bytes &payload ) -> Bytes;

private:
    static constexpr size_t _prefixSize{ 4 };

    size_t _maximumPayload;
    Bytes _buffer{};
};

}
