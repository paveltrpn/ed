import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Tire 1.0

import "../components"

Item {
    id: sceneInfoComponent

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    anchors {
        top: parent.top
        left: parent.left
        leftMargin: _units.scaled_1
        right: parent.right
        rightMargin: _units.scaled_1
    }

    height: visible ? (sceneInfoTabBar.height + sceneInfoTabLayout.implicitHeight) : 0
    clip: true

    TabBar {
        id: sceneInfoTabBar

        height: _units.scaled_24
        width: parent.width

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
            id: selectedInfoTab
            Layout.fillWidth: true
        }
    }
}
