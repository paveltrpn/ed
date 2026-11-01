#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <uv.h>

#include "io/async.h"
#include "io/schedulerbase.h"

namespace tire {

template <typename T>
struct TimeoutAwaitable;

template <typename T>
struct FileIOAwaitable;

template <typename T>
struct FilesystemWatchAwaitable;

struct TcpConnection;
struct TcpListener;
struct TcpConnectAwaitable;
struct TcpListenAwaitable;
struct TcpAcceptAwaitable;
struct TcpReadAwaitable;
struct TcpWriteAwaitable;

struct Scheduler final : SchedulerBase {
public:
    template <typename T>
    friend struct TimeoutAwaitable;

    template <typename T>
    friend struct FileIOAwaitable;

    template <typename T>
    friend struct FilesystemWatchAwaitable;

    friend struct TcpConnectAwaitable;
    friend struct TcpListenAwaitable;
    friend struct TcpAcceptAwaitable;
    friend struct TcpReadAwaitable;
    friend struct TcpWriteAwaitable;

public:
    using SchedulerBase::isLoopThread;

    // Fire once by timeout.
    auto timeout( uint64_t timeout ) -> TimeoutAwaitable<Task<void>>;

    //
    auto openFile( const std::string &path ) -> FileIOAwaitable<Task<void>>;

    //
    auto watchFile( const std::string &path ) -> FilesystemWatchAwaitable<Task<void>>;

    // Create a connection handle on the given loop.
    auto createTcpConnection( uv_loop_t *loop, int &status ) -> std::shared_ptr<TcpConnection>;

    // Connect to a TCP endpoint.
    auto connect( std::string host, uint16_t port ) -> TcpConnectAwaitable;

    // Start listening for TCP connections.
    auto listen( std::string host, uint16_t port ) -> TcpListenAwaitable;

    // Accept one incoming TCP connection.
    auto accept( const std::shared_ptr<TcpListener> &listener ) -> TcpAcceptAwaitable;

    // Read once from a TCP connection.
    auto read( const std::shared_ptr<TcpConnection> &connection ) -> TcpReadAwaitable;

    // Write bytes to a TCP connection.
    auto write( const std::shared_ptr<TcpConnection> &connection, std::vector<std::byte> bytes )
        -> TcpWriteAwaitable;

    // Close a TCP connection, canceling any pending read or write.
    auto close( const std::shared_ptr<TcpConnection> &connection ) -> void;

    // Close a TCP listener, canceling any pending listen or accept.
    auto close( const std::shared_ptr<TcpListener> &listener ) -> void;

private:
    // Submit callback to event loop.
    auto post( std::function<void( uv_loop_t * )> task ) -> void;

    // Submit callback to event loop.
    void schedule( std::function<void( uv_loop_t *, AsyncJobPayload * )> task, AsyncJobPayload *p );
};

}  // namespace tire
