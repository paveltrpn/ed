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

    property var buttonsList: []

    function deselectOthers(exceptItem) {
        for (let i in leftPanelMainComponent.buttonsList) {
            const btn = leftPanelMainComponent.buttonsList[i];
            if (btn !== exceptItem) {
                btn.checked = false;
            }
        }
    }

    Rectangle {
        id: leftPanelMainComponentWrapper
        anchors.fill: parent
        color: _color.button_shadow

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

        SIButtonMain {
            id: addObjectButton

            anchors {
                top: parent.top
                topMargin: leftPanelMainComponent._units.half
                left: parent.left
                leftMargin: leftPanelMainComponent._units.half
                right: parent.right
                rightMargin: leftPanelMainComponent._units.half
            }

            height: _units.scaled_56

            buttonLabel: "Add object"

            onClicked: {
                addObjectButton.checked = !addObjectButton.checked;

                if (addObjectButton.checked) {
                    leftPanelMainComponent.deselectOthers(addObjectButton);
                }
            }

            Component.onCompleted: {
                leftPanelMainComponent.buttonsList.push(addObjectButton);
            }
        }

        AddPrimitivesPopup {
            id: addPrimitivesPopupItem

            anchors {
                top: addObjectButton.bottom
                topMargin: visible ? leftPanelMainComponent._units.half : 0
                left: parent.left
                leftMargin: leftPanelMainComponent._units.half
                right: parent.right
                rightMargin: leftPanelMainComponent._units.half
            }

            visible: addObjectButton.checked
        }

        SIButtonMain {
            id: editObjectButton

            anchors {
                top: addPrimitivesPopupItem.bottom
                topMargin: leftPanelMainComponent._units.half
                left: parent.left
                leftMargin: leftPanelMainComponent._units.half
                right: parent.right
                rightMargin: leftPanelMainComponent._units.half
            }

            height: _units.scaled_56

            buttonLabel: "Edit object"

            onClicked: {
                editObjectButton.checked = !editObjectButton.checked;

                if (editObjectButton.checked) {
                    leftPanelMainComponent.deselectOthers(editObjectButton);
                }
            }

            Component.onCompleted: {
                leftPanelMainComponent.buttonsList.push(editObjectButton);
            }
        }

        SIButtonMain {
            id: infoButton

            anchors {
                top: editObjectButton.bottom
                topMargin: leftPanelMainComponent._units.half
                left: parent.left
                leftMargin: leftPanelMainComponent._units.half
                right: parent.right
                rightMargin: leftPanelMainComponent._units.half
            }

            height: _units.scaled_56

            buttonLabel: "Info"

            onClicked: {
                infoButton.checked = !infoButton.checked;

                if (infoButton.checked) {
                    leftPanelMainComponent.deselectOthers(infoButton);
                }
            }

            Component.onCompleted: {
                leftPanelMainComponent.buttonsList.push(infoButton);
            }
        }

        SceneInfoComponent {
            id: sceneInfoWidget
            anchors {
                top: infoButton.bottom
                topMargin: _units.half
                left: parent.left
                leftMargin: leftPanelMainComponent._units.half
                right: parent.right
                rightMargin: leftPanelMainComponent._units.half
            }

            visible: infoButton.checked
        }

        SIButtonMain {
            id: settingsButton

            anchors {
                bottom: parent.bottom
                left: parent.left
                leftMargin: leftPanelMainComponent._units.half
                right: parent.right
                rightMargin: leftPanelMainComponent._units.half
            }

            height: _units.scaled_56

            buttonLabel: "Settings"

            onClicked: {
                settingsButton.checked = !settingsButton.checked;

                if (settingsButton.checked) {
                    leftPanelMainComponent.deselectOthers(settingsButton);
                }
            }

            Component.onCompleted: {
                leftPanelMainComponent.buttonsList.push(settingsButton);
            }
        }

        SettingsComponent {
            id: settingsWidget
            anchors {
                bottom: settingsButton.top
                bottomMargin: _units.half
                left: parent.left
                leftMargin: leftPanelMainComponent._units.half
                right: parent.right
                rightMargin: leftPanelMainComponent._units.half
            }

            visible: settingsButton.checked
        }
    }
}
