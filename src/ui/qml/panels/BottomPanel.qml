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

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    color: _color.background

    // bottomLeftRadius:  _units.radiusEight
    // bottomRightRadius: _units.radiusEight

    Rectangle {
        id: bottomPanelMainComponentWrapper
        anchors.fill: parent
        color: _color.si_button_shadow
        bottomLeftRadius: _units.radiusEight
        bottomRightRadius: _units.radiusEight

        BottomPanelBg {
            id: bottomPanelBgItem

            anchors {
                top: parent.top
                topMargin: _units.scaled_8
                right: parent.right
                rightMargin: _units.scaled_8
                bottom: parent.bottom
                bottomMargin: _units.scaled_8
                left: parent.left
                leftMargin: _units.scaled_8
            }
        }
    }
}
