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

        SIButtonIndicator {
            id: dummyButton

            anchors {
                top: parent.top
                topMargin: rightPanelSystemComponent._units.half
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_scene
            height: _units.scaled_56
            buttonLabel: "DUMMY SYSTEM"

            onClicked: {}

            Component.onCompleted: {}
        }
    }
}
