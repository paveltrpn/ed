
#pragma

namespace tire {

enum class KeyAction { PRESS, RELEASE };
enum class MouseButton { LEFT, MIDDLE, RIGHT };

// ============================================================================
// ============== EventBase ===================================================
// ============================================================================

struct EventBase {
    virtual ~EventBase() = default;
};

// ============================================================================
// ============== EventKey ====================================================
// ============================================================================

struct EventKey : public EventBase {
    EventKey( KeyAction action, int key )
        : _action{ action }
        , _key{ key } {}

    KeyAction _action{};
    int _key{};
};

// ============================================================================
// ============== EventMouse ==================================================
// ============================================================================

struct EventMouse : public EventBase {
    KeyAction _action;
    MouseButton _button;
};

}  // namespace tire
