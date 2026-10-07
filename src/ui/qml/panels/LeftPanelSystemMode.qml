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

        MouseArea {
            id: closePopupsMouseArea
            anchors.fill: parent
            onClicked: {
                mainWindow.panelsActiveBtn = "";
            }
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
            buttonLabel: "About"

            checked: mainWindow.panelsActiveBtn === "aboutButton"

            onClicked: {
                if (aboutButton.checked) {
                    mainWindow.panelsActiveBtn = "";
                    return;
                }

                mainWindow.panelsActiveBtn = "aboutButton";
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
                mainWindow.panelsActiveBtn = "";
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

            checked: mainWindow.panelsActiveBtn === "uiSettingsButton"

            onClicked: {
                if (uiSettingsButton.checked) {
                    mainWindow.panelsActiveBtn = "";
                    return;
                }

                mainWindow.panelsActiveBtn = "uiSettingsButton";
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
                mainWindow.panelsActiveBtn = "";
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

            checked: mainWindow.panelsActiveBtn === "exitButton"

            onClicked: {
                if (exitButton.checked) {
                    mainWindow.panelsActiveBtn = "";
                    return;
                }

                mainWindow.panelsActiveBtn = "exitButton";
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
                mainWindow.panelsActiveBtn = "";
            }
        }
    }
}
