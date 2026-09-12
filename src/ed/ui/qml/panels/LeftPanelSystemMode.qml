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
    id: leftPanelSystemComponent

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    color: _color.si_button_shadow

    property var buttonsList: []

    function deselectOthers(exceptItem) {
        for (let i in leftPanelSystemComponent.buttonsList) {
            const btn = leftPanelSystemComponent.buttonsList[i];

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
            leftMargin: leftPanelSystemComponent._units.scaled_2
            rightMargin: leftPanelSystemComponent._units.scaled_2
        }

        SIButtonMain {
            id: aboutButton

            anchors {
                top: parent.top
                topMargin: leftPanelSystemComponent._units.half
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_system
            height: _units.scaled_56
            buttonLabel: "About"

            onClicked: {
                aboutButton.checked = !aboutButton.checked;

                if (aboutButton.checked) {
                    leftPanelSystemComponent.deselectOthers(aboutButton);
                }
            }

            Component.onCompleted: {
                leftPanelSystemComponent.buttonsList.push(aboutButton);
            }
        }

        AboutPanel {
            id: aboutPanelItem
            anchors {
                top: aboutButton.bottom
                topMargin: visible ? _units.half : 0
                left: parent.left
                right: parent.right
            }

            visible: aboutButton.checked

            onClose: {
                aboutWindow.show();
                leftPanelSystemComponent.deselectOthers(null);
            }

            SIWindow {
                id: aboutWindow

                width: 640
                height: 480
            }
        }

        SIButtonMain {
            id: exitButton

            anchors {
                bottom: exitPanelItem.top
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_system
            height: _units.scaled_56
            buttonLabel: "Exit"

            onClicked: {
                exitButton.checked = !exitButton.checked;

                if (exitButton.checked) {
                    leftPanelSystemComponent.deselectOthers(exitButton);
                }
            }

            Component.onCompleted: {
                leftPanelSystemComponent.buttonsList.push(exitButton);
            }
        }

        ExitPanel {
            id: exitPanelItem
            anchors {
                bottom: parent.bottom
                bottomMargin: visible ? _units.half : 0
                left: parent.left
                right: parent.right
            }

            visible: exitButton.checked

            onAccept: {
                MainWindow.quitApplication();
            }

            onDecline: {
                exitButton.checked = false;
            }
        }
    }
}
