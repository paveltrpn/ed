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

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    color: _color.si_button_shadow

    Item {
        id: rightPanelSystemModeWrapper
        anchors {
            fill: parent
            leftMargin: rightPanelSystemComponent._units.scaled_2
            rightMargin: rightPanelSystemComponent._units.scaled_2
        }

        MouseArea {
            id: leftEdgeMoveArea
            anchors {
                top: parent.top
                bottom: parent.bottom
                left: parent.left
            }
            width: 4
            cursorShape: Qt.SizeHorCursor
            onPressed: {
                MainWindow.resizeWindow(Qt.LeftEdge);
            }
        }
    }
}
