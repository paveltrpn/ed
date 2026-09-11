// qmllint disable unqualified
// qmllint disable Quick.property-changes-parsed

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Tire 1.0

import "../components"
import "../components/filedialog"

Rectangle {
    id: rightPanelMainComponent

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    color: _color.background

    // MouseArea {
    //     id: rightEdgeMoveArea
    //     anchors {
    //         top: parent.top
    //         right: parent.right
    //         bottom: parent.bottom
    //     }
    //     width: _units.scaled_6
    //     cursorShape: Qt.SizeHorCursor
    //     onPressed: {
    //         MainWindow.resizeWindow(Qt.RightEdge);
    //     }
    // }

    states: [
        State {
            name: "scene_mode"
            when: Tired.controlMode == 0
            PropertyChanges {
                target: rightPanelSceneModeItem
                visible: true
            }
            PropertyChanges {
                target: rightPanelSystemModeItem
                visible: false
            }
        },

        State {
            name: "system_mode"
            when: Tired.controlMode == 1
            PropertyChanges {
                target: rightPanelSceneModeItem
                visible: false
            }
            PropertyChanges {
                target: rightPanelSystemModeItem
                visible: true
            }
        }
    ]

    RightPanelSceneMode {
        id: rightPanelSceneModeItem
        anchors.fill: parent
    }

    RightPanelSystemMode {
        id: rightPanelSystemModeItem
        anchors.fill: parent
    }
}
