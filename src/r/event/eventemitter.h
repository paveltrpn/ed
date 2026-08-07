
#pragma

#include <atomic>
#include <mutex>
#include <vector>
#include <memory>

namespace tire {

struct EventObserver;
struct EventBase;

// ============================================================================
// ============== EventEmitter ================================================
// ============================================================================

struct EventEmitter {
    virtual void attach( std::weak_ptr<EventObserver> observer ) = 0;
    virtual void detach( std::weak_ptr<EventObserver> observer ) = 0;
    virtual void notify( std::shared_ptr<EventBase> event ) = 0;
    virtual ~EventEmitter() = default;
};

// ============================================================================
// ============== GlobalEventEmitter ==========================================
// ============================================================================

struct GlobalEventEmitter : public EventEmitter {
    GlobalEventEmitter( const GlobalEventEmitter &rhs ) = delete;
    GlobalEventEmitter( GlobalEventEmitter &&rhs ) = delete;
    auto operator=( const GlobalEventEmitter &rhs ) -> GlobalEventEmitter & = delete;
    auto operator=( GlobalEventEmitter &&rhs ) -> GlobalEventEmitter & = delete;

    static void init();
    [[nodiscard]] static GlobalEventEmitter &instance();

    void attach( std::weak_ptr<EventObserver> observer ) override;
    void detach( std::weak_ptr<EventObserver> observer ) override;
    void notify( std::shared_ptr<EventBase> event ) override;

private:
    GlobalEventEmitter();
    ~GlobalEventEmitter() = default;

    inline static std::atomic<GlobalEventEmitter *> _instance{ nullptr };
    inline static std::once_flag _initFlag;
    inline static bool _initSuccess{ false };

private:
    std::vector<std::weak_ptr<EventObserver>> _observers;
};

}  // namespace tire
