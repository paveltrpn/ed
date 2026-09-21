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
            id: addObjectButton

            anchors {
                top: parent.top
                topMargin: leftPanelSceneComponent._units.half
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_scene
            buttonLabel: "Add object"

            checked: leftPanelMainComponentWrapper.activeBtn === "addObjectButton"

            onClicked: {
                if (addObjectButton.checked) {
                    leftPanelMainComponentWrapper.activeBtn = ""
                    return
                }

                leftPanelMainComponentWrapper.activeBtn = "addObjectButton";
            }
        }

        AddPrimitivesPopup {
            id: addPrimitivesPopupItem

            anchors {
                top: addObjectButton.bottom
                topMargin: visible ? leftPanelSceneComponent._units.half : 0
                left: parent.left
                right: parent.right
            }

            visible: addObjectButton.checked
        }

        SIButtonMain {
            id: editObjectButton

            anchors {
                top: addPrimitivesPopupItem.bottom
                topMargin: leftPanelSceneComponent._units.half
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_scene
            buttonLabel: "Edit object"

            onClicked: {
                if (editObjectButton.checked) {
                    leftPanelMainComponentWrapper.activeBtn = ""
                    return
                }

                leftPanelMainComponentWrapper.activeBtn = "editObjectButton";
            }
        }

        SIButtonMain {
            id: deleteObjectButton

            anchors {
                top: editObjectButton.bottom
                topMargin: leftPanelSceneComponent._units.half
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_scene
            buttonLabel: "Delete object"

            onClicked: {
                leftPanelMainComponentWrapper.activeBtn = "deleteObjectButton";
            }
        }

        SIButtonMain {
            id: infoButton

            anchors {
                top: deleteObjectButton.bottom
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
                topMargin: _units.half
                left: parent.left
                right: parent.right
            }

            visible: infoButton.checked

            onClose: {
                leftPanelSceneComponent.deselectOthers(null);
            }
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

            onClose: {
                leftPanelSceneComponent.deselectOthers(null);
            }
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
