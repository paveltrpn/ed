import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Tire 1.0

import "../components"

Item {
    id: settingsComponent

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    anchors {
        top: parent.top
        left: parent.left
        leftMargin: _units.scaled_1
        right: parent.right
        rightMargin: _units.scaled_1
    }

    height: settingsTabBar.height + settingsTabLayout.implicitHeight
    clip: true

    TabBar {
        id: settingsTabBar

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        height: _units.scaled_24

        NpTabButton {
            text: qsTr("Grid")
            anchors.top: parent.top
            height: parent.height
            font: _fonts.label_accent
        }

        NpTabButton {
            text: qsTr("Test box")
            anchors.top: parent.top
            height: parent.height
            font: _fonts.label_accent
        }
    }

    StackLayout {
        id: settingsTabLayout

        anchors {
            top: settingsTabBar.bottom
            left: parent.left
            right: parent.right
        }

        implicitHeight: children[settingsTabBar.currentIndex].implicitHeight
        currentIndex: settingsTabBar.currentIndex

        GridTab {
            id: gridTab
            Layout.fillWidth: true
        }

        TestBoxTab {
            id: testBoxTab
            Layout.fillWidth: true
        }
    }
}
