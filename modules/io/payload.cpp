
module;

#include <string>

#include <uv.h>

export module io : payload;

namespace tire {

export struct AsyncJobPayload {
    void *coro{};
};

export struct TimeoutCbPayload final : public AsyncJobPayload {
    uint64_t timeout{};
};

export struct FileIOCbPayload final : public AsyncJobPayload {
    std::string path{};
    size_t opentResult;
};

export struct FilesystemWatchCbPayload final : public AsyncJobPayload {
    std::string path{};
    int status{};
    int events{};
};

}  // namespace tire