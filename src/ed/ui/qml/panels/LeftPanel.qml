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
    id: leftPanelMainComponent

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    color: _color.background

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

    states: [
        State {
            name: "scene_mode"
            when: Tired.controlMode == 0
            PropertyChanges {
                target: leftPanelSceneModeItem
                visible: true
            }
            PropertyChanges {
                target: leftPanelSystemModeItem
                visible: false
            }
        },

        State {
            name: "system_mode"
            when: Tired.controlMode == 1
            PropertyChanges {
                target: leftPanelSceneModeItem
                visible: false
            }
            PropertyChanges {
                target: leftPanelSystemModeItem
                visible: true
            }
        }
    ]

    LeftPanelSceneMode {
        id: leftPanelSceneModeItem
        anchors.fill: parent
    }

    LeftPanelSystemMode {
        id: leftPanelSystemModeItem
        anchors.fill: parent
    }
}
