#pragma once

#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <uv.h>

#include "io/async.h"

namespace tire {

struct TimeoutAwaitable;
struct TimeoutAwaitableState;

template <typename T>
struct FileIOAwaitable;

template <typename T>
struct FilesystemWatchAwaitable;

struct TcpConnection;
struct TcpListener;
struct TcpConnectAwaitable;
struct TcpConnectAwaitableState;
struct TcpListenAwaitable;
struct TcpAcceptAwaitable;
struct TcpReadAwaitable;
struct TcpWriteAwaitable;

// ======================================================================================
// ==================== Scheduler =======================================================
// ======================================================================================

struct Scheduler final {
public:
    friend struct TimeoutAwaitable;
    friend struct TimeoutAwaitableState;

    template <typename T>
    friend struct FileIOAwaitable;

    template <typename T>
    friend struct FilesystemWatchAwaitable;

    friend struct TcpConnectAwaitable;
    friend struct TcpConnectAwaitableState;
    friend struct TcpListenAwaitable;
    friend struct TcpAcceptAwaitable;
    friend struct TcpReadAwaitable;
    friend struct TcpWriteAwaitable;

    Scheduler();

    Scheduler( const Scheduler& other ) = delete;
    Scheduler( Scheduler&& other ) = delete;

    auto operator=( const Scheduler& other ) -> Scheduler& = delete;
    auto operator=( Scheduler&& other ) -> Scheduler& = delete;

    ~Scheduler();

    // Fire once by timeout.
    auto timeout( uint64_t timeout ) -> TimeoutAwaitable;

    //
    auto openFile( const std::string& path ) -> FileIOAwaitable<Task<void>>;

    //
    auto watchFile( const std::string& path ) -> FilesystemWatchAwaitable<Task<void>>;

    // Create a connection handle.
    auto initTcp( int& status ) -> std::shared_ptr<TcpConnection>;

    // Connect to a TCP endpoint.
    auto connect( std::string host, uint16_t port ) -> TcpConnectAwaitable;

    // Start listening for TCP connections.
    auto listen( std::string host, uint16_t port ) -> TcpListenAwaitable;

    // Accept one incoming TCP connection.
    auto accept( const std::shared_ptr<TcpListener>& listener ) -> TcpAcceptAwaitable;

    // Read once from a TCP connection.
    auto read( const std::shared_ptr<TcpConnection>& connection ) -> TcpReadAwaitable;

    // Write bytes to a TCP connection.
    auto write( const std::shared_ptr<TcpConnection>& connection, std::vector<std::byte> bytes ) -> TcpWriteAwaitable;

    // Close a TCP connection, canceling any pending read or write.
    auto close( const std::shared_ptr<TcpConnection>& connection ) -> void;

    // Close a TCP listener, canceling any pending listen or accept.
    auto close( const std::shared_ptr<TcpListener>& listener ) -> void;

private:
    static auto asyncCallback( uv_async_t* handle ) -> void;

    auto dequeue() -> std::optional<std::function<void( uv_loop_t* )>>;
    auto stop() -> void;

    // Submit callback to event loop.
    auto schedule( std::function<void( uv_loop_t* )> task ) -> void;

private:
    // Это единственный хендл, который живёт всё время работы loop'а и держит uv_run от возврата.
    uv_async_t _asyncWakeup{};
    uv_loop_t* _loop{};

    std::unique_ptr<std::thread> _thread{};
    std::mutex _startupMutex{};
    std::condition_variable _startupCondition{};

    bool _startupComplete{ false };
    int _startupStatus{ 0 };

    std::mutex _jobMutex{};
    std::deque<std::function<void( uv_loop_t* )>> _jobs{};
};

}  // namespace tire
