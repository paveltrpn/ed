import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Tire 1.0

import "../components"

Rectangle {
    id: sceneInfoComponent

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    height: visible ? (sceneInfoTabBar.height + sceneInfoTabLayout.implicitHeight) : 0

    radius: _units.radiusEight

    color: _color.popup_background

    Behavior on height {
        NumberAnimation {
            duration: 200
        }
    }

    clip: true

    TabBar {
        id: sceneInfoTabBar

        height: _units.scaled_24
        width: parent.width

        NpTabButton {
            text: qsTr("Scene total")

            anchors.top: parent.top
            height: parent.height

            topLeftRadius: _units.radiusEight

            font: _fonts.label_accent
        }

        NpTabButton {
            text: qsTr("Current group")

            anchors.top: parent.top
            height: parent.height

            topRightRadius: _units.radiusEight

            font: _fonts.label_accent
        }
    }

    StackLayout {
        id: sceneInfoTabLayout

        anchors {
            top: sceneInfoTabBar.bottom
            left: parent.left
            right: parent.right
        }

        implicitHeight: children[sceneInfoTabBar.currentIndex].implicitHeight
        currentIndex: sceneInfoTabBar.currentIndex

        SceneTotalInfoTab {
            id: sceneTotalInfoTab
            Layout.fillWidth: true
        }

        CurrentGroupTab {
            id: currentGroupTab
            Layout.fillWidth: true
        }
    }
}
