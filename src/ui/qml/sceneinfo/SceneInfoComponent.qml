// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Tire 1.0

import "../components"

Rectangle {
    id: sceneInfoComponent

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    color: _color.si_background_light

    border {
        width: _units.scaled_1
        color: _color.si_background_dark
    }

    anchors.fill: parent

    Rectangle {
        id: tabsArea

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: bottomPanelArea.top
            margins: _units.scaled_4
        }

        border {
            width: _units.scaled_1
            color: _color.si_background_dark
        }

        TabBar {
            id: sceneInfoTabBar

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                margins: _units.scaled_4
            }

            height: _units.scaled_24

            NpTabButton {
                text: qsTr("Scene")
                anchors.top: parent.top
                height: parent.height
                font: _fonts.label_accent
            }

            NpTabButton {
                text: qsTr("Selected")
                anchors.top: parent.top
                height: parent.height
                font: _fonts.label_accent
            }
        }

        StackLayout {
            id: sceneInfoTabLayout

            anchors {
                top: sceneInfoTabBar.bottom
                bottom: parent.bottom
                left: parent.left
                right: parent.right
                margins: _units.scaled_4
            }

            implicitHeight: children[sceneInfoTabBar.currentIndex].implicitHeight
            currentIndex: sceneInfoTabBar.currentIndex

            SceneTotalInfoTab {
                id: sceneTotalInfoTab
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            CurrentGroupTab {
                id: selectedInfoTab
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }

    Rectangle {
        id: bottomPanelArea

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: _units.scaled_4
        }

        height: 40

        border {
            width: _units.scaled_1
            color: _color.si_background_dark
        }

        SIButton {
            id: closeButton

            anchors {
                right: parent.right
                bottom: parent.bottom
                margins: _units.scaled_4
            }

            height: _units.scaled_32
            width: _units.scaled_64

            buttonLabel: "Close"

            onClicked: {
                siEmbeddedWidgetComponent.aboutToClose();
            }
        }
    }
}
