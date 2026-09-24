#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <memory>
#include <typeindex>
#include <string>
#include <algorithm>

namespace tire {

// ============================================================
// 1. Subscription handle — RAII, allows unsubscribing
// ============================================================
class Subscription {
public:
    Subscription() = default;
    explicit Subscription( std::function<void()> unsub )
        : unsub_( std::move( unsub ) ) {}

    Subscription( const Subscription& ) = delete;
    Subscription& operator=( const Subscription& ) = delete;

    Subscription( Subscription&& o ) noexcept
        : unsub_( std::move( o.unsub_ ) ) {
        o.unsub_ = nullptr;
    }

    Subscription& operator=( Subscription&& o ) noexcept {
        if ( this != &o ) {
            reset();
            unsub_ = std::move( o.unsub_ );
            o.unsub_ = nullptr;
        }
        return *this;
    }

    ~Subscription() { reset(); }

    void reset() {
        if ( unsub_ ) {
            unsub_();
            unsub_ = nullptr;
        }
    }

private:
    std::function<void()> unsub_;
};

// ============================================================
// 2. MessageBus — the core
// ============================================================
class MessageBus {
public:
    using HandlerId = std::size_t;

    // Subscribe to messages of type T
    template <typename T, typename F>
    Subscription subscribe( F&& callback ) {
        std::lock_guard<std::mutex> lock( mutex_ );
        auto key = std::type_index( typeid( T ) );
        HandlerId id = next_id_++;

        auto handler = [cb = std::forward<F>( callback )]( const void* msg ) { cb( *static_cast<const T*>( msg ) ); };

        handlers_[key].push_back( { id, std::move( handler ) } );

        return Subscription( [this, key, id]() { unsubscribe( key, id ); } );
    }

    // Publish a message of type T
    template <typename T>
    void publish( const T& message ) {
        std::vector<Handler> snapshot;
        {
            std::lock_guard<std::mutex> lock( mutex_ );
            auto it = handlers_.find( std::type_index( typeid( T ) ) );
            if ( it == handlers_.end() ) return;
            snapshot = it->second;  // copy handlers so we can call outside lock
        }

        for ( auto& h : snapshot ) {
            h.fn( &message );
        }
    }

    // Convenience: publish a temporary
    template <typename T>
    void emit( T&& message ) {
        publish<T>( std::forward<T>( message ) );
    }

private:
    struct Handler {
        HandlerId id;
        std::function<void( const void* )> fn;
    };

    void unsubscribe( std::type_index key, HandlerId id ) {
        std::lock_guard<std::mutex> lock( mutex_ );
        auto it = handlers_.find( key );
        if ( it == handlers_.end() ) return;
        auto& vec = it->second;
        vec.erase( std::remove_if( vec.begin(), vec.end(), [id]( const Handler& h ) { return h.id == id; } ),
                   vec.end() );
        if ( vec.empty() ) handlers_.erase( it );
    }

    std::mutex mutex_;
    std::unordered_map<std::type_index, std::vector<Handler>> handlers_;
    HandlerId next_id_ = 1;
};

// ============================================================
// 3. A helper "Node" — object that is both producer & consumer
// ============================================================
class Node {
public:
    explicit Node( std::string name, MessageBus& bus )
        : name_( std::move( name ) )
        , bus_( bus ) {}

    virtual ~Node() = default;

    const std::string& name() const { return name_; }

    template <typename T>
    void publish( const T& msg ) {
        bus_.publish<T>( msg );
    }

protected:
    std::string name_;
    MessageBus& bus_;
    std::vector<Subscription> subs_;  // keeps subscriptions alive
};
}  // namespace tire
