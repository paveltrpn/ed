import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Tire 1.0

import "../components"

Rectangle {
    id: settingsComponent

    readonly property var _fonts: Appearence.fonts
    readonly property var _gaps: Appearence.gaps
    readonly property var _radius: Appearence.radius
    readonly property var _color: Appearence.colors

    height: visible ? settingsTabBar.height + settingsTabLayout.implicitHeight : 0

    radius: 2

    color: _color.popup_background

    border {
        width: 2
        color: _color.popup_header
    }

    Behavior on height {
        NumberAnimation {
            duration: 200
        }
    }

    clip: true

    TabBar {
        id: settingsTabBar

        height: 24
        width: parent.width

        NpTabButton {
            text: qsTr("Grid")

            anchors.top: parent.top
            height: parent.height

            topLeftRadius: 2

            font: _fonts.label_accent
        }

        NpTabButton {
            text: qsTr("Test box")

            anchors.top: parent.top
            height: parent.height

            topRightRadius: 2

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
