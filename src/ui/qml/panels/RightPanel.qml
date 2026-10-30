// qmllint disable unqualified
// qmllint disable Quick.property-changes-parsed

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Tire 1.0

import "../components"

Rectangle {
    id: rightPanelMainComponent

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    color: _color.background

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
            PropertyChanges {
                target: rightPanelReNderModeItem
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
            PropertyChanges {
                target: rightPanelReNderModeItem
                visible: false
            }
        },
        State {
            name: "render_mode"
            when: Tired.controlMode == 2
            PropertyChanges {
                target: rightPanelSceneModeItem
                visible: false
            }
            PropertyChanges {
                target: rightPanelSystemModeItem
                visible: false
            }
            PropertyChanges {
                target: rightPanelReNderModeItem
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

    RightPanelRenderMode {
        id: rightPanelReNderModeItem
        anchors.fill: parent
    }
}
