#pragma once

// Portions of this code are derived from vsgQt, licensed under the MIT License.
// See: https:/https://github.com/vsg-dev/vsgQt/LICENSE

#include <QKeyEvent>

#include <vsg/all.h>

namespace tire {

// ====================================================================
// ========== KeyboardMap =============================================
// ====================================================================

class KeyboardMap : public vsg::Inherit<vsg::Object, KeyboardMap> {
public:
    KeyboardMap();

    bool getKeySymbol( const QKeyEvent* e, vsg::KeySymbol& keySymbol, vsg::KeySymbol& modifiedKeySymbol,
                       vsg::KeyModifier& keyModifier );

protected:
    using VirtualKeyToKeySymbolMap = std::map<uint32_t, vsg::KeySymbol>;
    VirtualKeyToKeySymbolMap _keycodeMap;
};

}  // namespace tire

EVSG_type_name( tire::KeyboardMap );
