#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include <uv.h>

namespace tire {

struct AsyncJobPayload {
    void *coro{};
};

struct TimeoutCbPayload final : public AsyncJobPayload {
    uint64_t timeout{};
};

struct FileIOCbPayload final : public AsyncJobPayload {
    std::string path{};
    size_t opentResult;
};

struct FilesystemWatchCbPayload final : public AsyncJobPayload {
    std::string path{};
    int status{};
    int events{};
};

}  // namespace tire
