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
    id: leftPanelSceneComponent

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    color: _color.si_button_shadow

    Item {
        id: leftPanelMainComponentWrapper
        anchors {
            fill: parent
            leftMargin: leftPanelSceneComponent._units.scaled_2
            rightMargin: leftPanelSceneComponent._units.scaled_2
        }

        property string activeBtn: ""

        SIButtonMain {
            id: objectAddButton

            anchors {
                top: parent.top
                topMargin: leftPanelSceneComponent._units.half
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_scene
            buttonLabel: "Object"

            checked: leftPanelMainComponentWrapper.activeBtn === "objectAddButton"

            onClicked: {
                if (objectAddButton.checked) {
                    leftPanelMainComponentWrapper.activeBtn = ""
                    return
                }

                leftPanelMainComponentWrapper.activeBtn = "objectAddButton";
            }
        }

        AddObjectPopup {
            id: addObjectPopupItem

            anchors {
                top: objectAddButton.bottom
                topMargin: visible ? _units.eight : 0
                left: parent.left
                right: parent.right
            }

            visible: objectAddButton.checked
        }

        SIButtonMain {
            id: meshAddButton

            anchors {
                top: addObjectPopupItem.bottom
                topMargin: leftPanelSceneComponent._units.half
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_scene
            buttonLabel: "Mesh"

            checked: leftPanelMainComponentWrapper.activeBtn === "meshAddButton"

            onClicked: {
                if (meshAddButton.checked) {
                    leftPanelMainComponentWrapper.activeBtn = ""
                    return
                }

                leftPanelMainComponentWrapper.activeBtn = "meshAddButton";
            }
        }

        SIButtonMain {
            id: bezierAddButton

            anchors {
                top: meshAddButton.bottom
                topMargin: leftPanelSceneComponent._units.half
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_scene
            buttonLabel: "Bezier"

            checked: leftPanelMainComponentWrapper.activeBtn === "bezierAddButton"

            onClicked: {
                if (bezierAddButton.checked) {
                    leftPanelMainComponentWrapper.activeBtn = ""
                    return
                }

                leftPanelMainComponentWrapper.activeBtn = "bezierAddButton";
            }
        }

        SIButtonMain {
            id: implicitAddButton

            anchors {
                top: bezierAddButton.bottom
                topMargin: leftPanelSceneComponent._units.half
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_scene
            buttonLabel: "Implicit"

            checked: leftPanelMainComponentWrapper.activeBtn === "implicitAddButton"

            onClicked: {
                if (implicitAddButton.checked) {
                    leftPanelMainComponentWrapper.activeBtn = ""
                    return
                }

                leftPanelMainComponentWrapper.activeBtn = "implicitAddButton";
            }
        }

        SIButtonMain {
            id: landscapeAddButton

            anchors {
                top: implicitAddButton.bottom
                topMargin: leftPanelSceneComponent._units.half
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_scene
            buttonLabel: "Landscape"

            checked: leftPanelMainComponentWrapper.activeBtn === "landscapeAddButton"

            onClicked: {
                if (landscapeAddButton.checked) {
                    leftPanelMainComponentWrapper.activeBtn = ""
                    return
                }

                leftPanelMainComponentWrapper.activeBtn = "landscapeAddButton";
            }
        }

        SIButtonMain {
            id: polytopeAddButton

            anchors {
                top: landscapeAddButton.bottom
                topMargin: leftPanelSceneComponent._units.half
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_scene
            buttonLabel: "Polytope"

            checked: leftPanelMainComponentWrapper.activeBtn === "polytopeAddButton"

            onClicked: {
                if (polytopeAddButton.checked) {
                    leftPanelMainComponentWrapper.activeBtn = ""
                    return
                }

                leftPanelMainComponentWrapper.activeBtn = "polytopeAddButton";
            }
        }

        SIButtonMain {
            id: infoButton

            anchors {
                top: polytopeAddButton.bottom
                topMargin: leftPanelSceneComponent._units.half
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_scene
            buttonLabel: "Info"

            checked: leftPanelMainComponentWrapper.activeBtn === "infoButton"

            onClicked: {
                if (infoButton.checked) {
                    leftPanelMainComponentWrapper.activeBtn = ""
                    return
                }

                leftPanelMainComponentWrapper.activeBtn = "infoButton";
            }
        }

        SceneInfoPanel {
            id: sceneinfoPanelItem
            anchors {
                top: infoButton.bottom
                topMargin: _units.eight
                left: parent.left
                right: parent.right
            }

            visible: infoButton.checked
        }

        SIButtonMain {
            id: settingsButton

            anchors {
                bottom: settingsPanelItem.top
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_scene
            buttonLabel: "Settings"

            checked: leftPanelMainComponentWrapper.activeBtn === "settingsButton"

            onClicked: {
                if (settingsButton.checked) {
                    leftPanelMainComponentWrapper.activeBtn = ""
                    return
                }

                leftPanelMainComponentWrapper.activeBtn = "settingsButton";
            }
        }

        SettingsPanel {
            id: settingsPanelItem
            anchors {
                bottom: parent.bottom
                bottomMargin: visible ? _units.half : 0
                left: parent.left
                right: parent.right
            }

            visible: settingsButton.checked
        }

        // SettingsComponent {
        //     id: settingsWidget
        //     anchors {
        //         bottom: settingsButton.top
        //         bottomMargin: _units.half
        //         left: parent.left
        //         right: parent.right
        //     }

        //     visible: settingsButton.checked
        // }
    }
}
