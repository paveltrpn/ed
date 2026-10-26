#include "io/filesystemwatch.h"

namespace tire {

auto Scheduler::watchFile( const std::string &path ) -> FilesystemWatchAwaitable<Task<void>> {
    //
    return FilesystemWatchAwaitable<Task<void>>{ /* Context */ *this, path };
};

}  // namespace tire
