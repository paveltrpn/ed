#include "std/threadpool.h"

namespace tire {

Task::Task( const Task &other )
    : total_predecessors_{ other.total_predecessors_ }
    , func_{ other.func_ }
    , next_{ other.next_ } {
    remaining_predecessors_.store( other.remaining_predecessors_ );
    cancellation_flags_.store( other.cancellation_flags_ );
}

Task::Task( Task &&other ) noexcept
    : total_predecessors_{ other.total_predecessors_ }
    , func_{ std::move( other.func_ ) }
    , next_{ std::move( other.next_ ) } {
    remaining_predecessors_.store( other.remaining_predecessors_ );
    cancellation_flags_.store( other.cancellation_flags_ );
}

Task &Task::operator=( const Task &other ) {
    total_predecessors_ = other.total_predecessors_;
    remaining_predecessors_.store( other.remaining_predecessors_ );
    cancellation_flags_.store( other.cancellation_flags_ );
    func_ = other.func_;
    next_ = other.next_;
    return *this;
}

Task &Task::operator=( Task &&other ) noexcept {
    total_predecessors_ = other.total_predecessors_;
    remaining_predecessors_.store( other.remaining_predecessors_ );
    cancellation_flags_.store( other.cancellation_flags_ );
    func_ = std::move( other.func_ );
    next_ = std::move( other.next_ );
    return *this;
}

void Task::Succeed( Task *task ) {
    task->next_.push_back( this );
    ++total_predecessors_;
    remaining_predecessors_.fetch_add( 1 );
}

void Task::Precede( Task *task ) {
    next_.push_back( task );
    ++task->total_predecessors_;
    task->remaining_predecessors_.fetch_add( 1 );
}

bool Task::Cancel() {
    return ( cancellation_flags_.fetch_or( kCancelled ) & kInvoked ) == 0;
}

void Task::Reset() {
    cancellation_flags_.store( 0 );
}

ThreadPool::ThreadPool( const unsigned threads_count )
    : queues_count_{ threads_count + 1 }
    , queues_{ threads_count + 1 } {
    threads_.reserve( threads_count );
    for ( unsigned i = 0; i != threads_count; ++i ) {
        threads_.emplace_back( [this, i] -> void { run( i + 1 ); } );
    }
}

ThreadPool::~ThreadPool() noexcept {
    wait();
    stop_.test_and_set();
    tasks_count_ += queues_count_;
    tasks_count_.notify_all();
    for ( auto &thread : threads_ ) {
        thread.join();
    }
}

auto ThreadPool::submit( Task *task ) -> void {
    ++tasks_count_;
    queues_[index_].Push( task );
    tasks_count_.notify_one();
}

auto ThreadPool::wait() const -> void {
    while ( const auto count = tasks_count_.load() ) {
        tasks_count_.wait( count );
    }
}

auto ThreadPool::run( const unsigned i ) -> void {
    index_ = i;
    for ( auto attempts = 0;; ) {
        if ( constexpr auto max_attempts = 100; ++attempts > max_attempts ) {
            tasks_count_.wait( 0 );
        }
        if ( auto *task = getTask() ) {
            execute( task );
            attempts = 0;
        } else if ( stop_.test() ) {
            return;
        }
    }
}

auto ThreadPool::execute( Task *task ) -> void {
    for ( Task *next = nullptr; task; next = nullptr ) {
        task->remaining_predecessors_.store( task->total_predecessors_ );
        if ( task->cancellation_flags_.fetch_or( kInvoked ) & kCancelled ) {
            break;
        }
        if ( task->func_ ) {
            task->func_();
        }
        auto it = task->next_.begin();
        for ( ; it != task->next_.end(); ++it ) {
            if ( ( *it )->remaining_predecessors_.fetch_sub( 1 ) == 1 ) {
                next = *it++;
                break;
            }
        }
        for ( ; it != task->next_.end(); ++it ) {
            if ( ( *it )->remaining_predecessors_.fetch_sub( 1 ) == 1 ) {
                submit( *it );
            }
        }
        if ( task->delete_ ) {
            delete task;
        }
        task = next;
    }
    if ( tasks_count_.fetch_sub( 1 ) == 1 ) {
        tasks_count_.notify_all();
    }
}

auto ThreadPool::getTask() -> Task * {
    const auto i = index_;
    auto *task = queues_[i].Pop();
    if ( task ) {
        return task;
    }
    for ( unsigned j = 1; j != queues_count_; ++j ) {
        task = queues_[( i + j ) % queues_count_].Steal();
        if ( task ) {
            return task;
        }
    }
    return nullptr;
}

thread_local unsigned ThreadPool::index_{ 0 };

}  // namespace tire
