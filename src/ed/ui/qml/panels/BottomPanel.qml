// qmllint disable unqualified
// qmllint disable Quick.property-changes-parsed

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Tire 1.0

import "../components"
import "../components/filedialog"

Rectangle {
    id: bottomPanelMainComponent

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    color: _color.background

    // bottomLeftRadius:  _units.radiusEight
    // bottomRightRadius: _units.radiusEight

    Rectangle {
        id: bottomPanelMainComponentWrapper
        anchors.fill: parent
        color: _color.si_button_shadow
        bottomLeftRadius: _units.radiusEight
        bottomRightRadius: _units.radiusEight
    }
}
