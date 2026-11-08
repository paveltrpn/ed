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

    property string modeColor: _color.si_mode_system

    function centeredWidgetPosition(widget) {
        const center = tiredUI.mainWindowCenter;

        return Qt.point(center.x - widget.width / 2, center.y - widget.height / 2);
    }

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
                topMargin: leftPanelSystemComponent._units.quarter
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: leftPanelSystemComponent.modeColor
            buttonLabel: "About"

            onClicked: {
                if (!aboutWidget.visible) {
                    aboutWidget.visible = true;
                }
            }

            SIEmbeddedWidget {
                id: aboutWidget

                visible: false

                width: 640
                height: 480

                title: "About"

                contentSource: "../panels/AboutComponent.qml"

                x: centeredWidgetPosition(aboutWidget).x
                y: centeredWidgetPosition(aboutWidget).y
            }
        }

        SIButtonMain {
            id: vulkanInfoButton

            anchors {
                top: aboutButton.bottom
                topMargin: _units.half
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: leftPanelSystemComponent.modeColor
            buttonLabel: "Vulkan Info"

            onClicked: {
                if (!vulkanInfoWidget.visible) {
                    vulkanInfoWidget.visible = true;
                }
            }

            SIEmbeddedWidget {
                id: vulkanInfoWidget

                visible: false

                width: 640
                height: 480

                title: "Vulkan Info"

                contentSource: "../panels/VulkanInfoComponent.qml"

                x: centeredWidgetPosition(vulkanInfoWidget).x
                y: centeredWidgetPosition(vulkanInfoWidget).y
            }
        }

        SIButtonMain {
            id: uiSettingsButton

            anchors {
                top: vulkanInfoButton.bottom
                topMargin: _units.half
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: leftPanelSystemComponent.modeColor
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
            id: settingsButton

            anchors {
                bottom: settingsPanelItem.top
                bottomMargin: _units.quarter
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: leftPanelSystemComponent.modeColor
            buttonLabel: "Settings"

            checked: mainWindow.panelsActiveBtn === "settingsButton"

            onClicked: {
                if (settingsButton.checked) {
                    mainWindow.panelsActiveBtn = "";
                    return;
                }

                mainWindow.panelsActiveBtn = "settingsButton";
            }
        }

        SettingsPanel {
            id: settingsPanelItem
            anchors {
                bottom: exitButton.top
                bottomMargin: visible ? _units.half : 0
                left: parent.left
                right: parent.right
            }

            visible: settingsButton.checked
        }

        SIButtonMain {
            id: exitButton

            anchors {
                bottom: exitPanelItem.top
                bottomMargin: _units.quarter
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: leftPanelSystemComponent.modeColor
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
                bottomMargin: visible ? _units.quarter : 0
                left: parent.left
                right: parent.right
            }

            visible: exitButton.checked

            onAccept: {
                tiredUI.quitApplication();
            }

            onDecline: {
                mainWindow.panelsActiveBtn = "";
            }
        }
    }
}
