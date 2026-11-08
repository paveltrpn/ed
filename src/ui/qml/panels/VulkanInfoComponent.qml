// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"
import "vulkaninfo"

Rectangle {
    id: vulkanInfoComponent

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
            id: vulkanInfoTabBar

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                margins: _units.scaled_4
            }

            height: _units.scaled_24

            NpTabButton {
                text: qsTr("Instance")
                anchors.top: parent.top
                height: parent.height
                font: _fonts.label_accent
            }

            NpTabButton {
                text: qsTr("Physical device")
                anchors.top: parent.top
                height: parent.height
                font: _fonts.label_accent
            }

            NpTabButton {
                text: qsTr("Features")
                anchors.top: parent.top
                height: parent.height
                font: _fonts.label_accent
            }

            NpTabButton {
                text: qsTr("Extensions")
                anchors.top: parent.top
                height: parent.height
                font: _fonts.label_accent
            }

            NpTabButton {
                text: qsTr("Queues")
                anchors.top: parent.top
                height: parent.height
                font: _fonts.label_accent
            }

            NpTabButton {
                text: qsTr("Memory")
                anchors.top: parent.top
                height: parent.height
                font: _fonts.label_accent
            }

            NpTabButton {
                text: qsTr("Device")
                anchors.top: parent.top
                height: parent.height
                font: _fonts.label_accent
            }
        }

        StackLayout {
            id: vulkanInfoTabLayout

            anchors {
                top: vulkanInfoTabBar.bottom
                bottom: parent.bottom
                left: parent.left
                right: parent.right
                margins: _units.scaled_4
            }

            implicitHeight: children[vulkanInfoTabBar.currentIndex].implicitHeight
            currentIndex: vulkanInfoTabBar.currentIndex

            InstanceInfoTab {
                id: instanceInfoTab
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            PhysicalDeviceInfoTab {
                id: physicalDeviceInfoTab
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            FeaturesInfoTab {
                id: featuresInfoTab
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            ExtensionsInfoTab {
                id: extensionsInfoTab
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            QueuesInfoTab {
                id: queuesInfoTab
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            MemoryInfoTab {
                id: memoryInfoTab
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            DeviceInfoTab {
                id: deviceInfoTab
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
