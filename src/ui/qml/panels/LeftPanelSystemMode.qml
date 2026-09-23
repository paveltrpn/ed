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

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    color: _color.si_button_shadow

    Item {
        id: leftPanelMainComponentWrapper
        anchors {
            fill: parent
            leftMargin: leftPanelSystemComponent._units.scaled_2
            rightMargin: leftPanelSystemComponent._units.scaled_2
        }

        property string activeBtn: ""

        SIButtonMain {
            id: aboutButton

            anchors {
                top: parent.top
                topMargin: leftPanelSystemComponent._units.half
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_system
            buttonLabel: "About"

            checked: leftPanelMainComponentWrapper.activeBtn === "aboutButton"

            onClicked: {
                if (aboutButton.checked) {
                    leftPanelMainComponentWrapper.activeBtn = "";
                    return;
                }

                leftPanelMainComponentWrapper.activeBtn = "aboutButton";
            }
        }

        AboutPanel {
            id: aboutPanelItem

            anchors {
                top: aboutButton.bottom
                topMargin: visible ? _units.eight : 0
                left: parent.left
                right: parent.right
            }

            visible: aboutButton.checked

            onClose: {
                leftPanelMainComponentWrapper.activeBtn = "";
            }
        }

        SIButtonMain {
            id: uiSettingsButton

            anchors {
                top: aboutPanelItem.bottom
                topMargin: aboutPanelItem._units.half
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_system
            buttonLabel: "Ui"

            checked: leftPanelMainComponentWrapper.activeBtn === "uiSettingsButton"

            onClicked: {
                if (uiSettingsButton.checked) {
                    leftPanelMainComponentWrapper.activeBtn = "";
                    return;
                }

                leftPanelMainComponentWrapper.activeBtn = "uiSettingsButton";
            }
        }

        UiSettingsPanel {
            id: uiSettingsPanelItem

            anchors {
                top: uiSettingsButton.bottom
                topMargin: visible ? _units.eight : 0
                left: parent.left
                right: parent.right
            }

            visible: uiSettingsButton.checked

            onClose: {
                leftPanelMainComponentWrapper.activeBtn = "";
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
            buttonLabel: "Exit"

            checked: leftPanelMainComponentWrapper.activeBtn === "exitButton"

            onClicked: {
                if (exitButton.checked) {
                    leftPanelMainComponentWrapper.activeBtn = "";
                    return;
                }

                leftPanelMainComponentWrapper.activeBtn = "exitButton";
            }
        }

        ExitPanel {
            id: exitPanelItem
            anchors {
                bottom: parent.bottom
                bottomMargin: visible ? _units.eight : 0
                left: parent.left
                right: parent.right
            }

            visible: exitButton.checked

            onAccept: {
                mainWindow.quitApplication();
            }

            onDecline: {
                leftPanelMainComponentWrapper.activeBtn = "";
            }
        }
    }
}
