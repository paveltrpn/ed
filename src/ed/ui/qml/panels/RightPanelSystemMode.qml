// qmllint disable unqualified
// qmllint disable Quick.property-changes-parsed

import QtQuick
import QtQuick.Layouts

import Tire 1.0

import "../components"
import "../sceneinfo"
import "../addforms"
import "../settings"

Rectangle {
    id: rightPanelSystemComponent

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    color: _color.si_button_shadow

    Item {
        id: rightPanelSystemModeWrapper
        anchors {
            fill: parent
            leftMargin: rightPanelSystemComponent._units.scaled_2
            rightMargin: rightPanelSystemComponent._units.scaled_2
        }
    }
}
