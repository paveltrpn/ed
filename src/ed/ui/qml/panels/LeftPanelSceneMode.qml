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

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    color: _color.si_button_shadow

    property var buttonsList: []

    function deselectOthers(exceptItem) {
        for (let i in leftPanelSceneComponent.buttonsList) {
            const btn = leftPanelSceneComponent.buttonsList[i];

            if (exceptItem === null) {
                btn.checked = false;
                continue;
            }

            if (btn !== exceptItem) {
                btn.checked = false;
            }
        }
    }

    Item {
        id: leftPanelMainComponentWrapper
        anchors {
            fill: parent
            leftMargin: leftPanelSceneComponent._units.scaled_2
            rightMargin: leftPanelSceneComponent._units.scaled_2
        }

        SIButtonMain {
            id: addObjectButton

            anchors {
                top: parent.top
                topMargin: leftPanelSceneComponent._units.half
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_scene
            height: _units.scaled_56
            buttonLabel: "Add object"

            onClicked: {
                addObjectButton.checked = !addObjectButton.checked;

                if (addObjectButton.checked) {
                    leftPanelSceneComponent.deselectOthers(addObjectButton);
                }
            }

            Component.onCompleted: {
                leftPanelSceneComponent.buttonsList.push(addObjectButton);
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
            height: _units.scaled_56
            buttonLabel: "Edit object"

            onClicked: {
                editObjectButton.checked = !editObjectButton.checked;

                if (editObjectButton.checked) {
                    leftPanelSceneComponent.deselectOthers(editObjectButton);
                }
            }

            Component.onCompleted: {
                leftPanelSceneComponent.buttonsList.push(editObjectButton);
            }
        }

        SIButtonMain {
            id: infoButton

            anchors {
                top: editObjectButton.bottom
                topMargin: leftPanelSceneComponent._units.half
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_scene
            height: _units.scaled_56
            buttonLabel: "Info"

            onClicked: {
                infoButton.checked = !infoButton.checked;

                if (infoButton.checked) {
                    leftPanelSceneComponent.deselectOthers(infoButton);
                }
            }

            Component.onCompleted: {
                leftPanelSceneComponent.buttonsList.push(infoButton);
            }
        }

        SceneInfoComponent {
            id: sceneInfoWidget
            anchors {
                top: infoButton.bottom
                topMargin: _units.half
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
            height: _units.scaled_56
            buttonLabel: "Settings"

            onClicked: {
                settingsButton.checked = !settingsButton.checked;

                if (settingsButton.checked) {
                    leftPanelSceneComponent.deselectOthers(settingsButton);
                }
            }

            Component.onCompleted: {
                leftPanelSceneComponent.buttonsList.push(settingsButton);
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
                settingsWindow.show();
                leftPanelSceneComponent.deselectOthers(null);
            }

            SIWindow {
                id: settingsWindow

                width: 640
                height: 480
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
