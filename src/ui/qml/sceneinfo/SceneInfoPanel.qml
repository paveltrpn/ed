// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: sceneinfoPanelComponent

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    color: _color.si_background_light
    radius: _units.radiusEight
    height: visible ? sceneinfoPanelButtonsLayout.implicitHeight : 0

    signal close

    Behavior on height {
        NumberAnimation {
            duration: 50
        }
    }

    ColumnLayout {
        id: sceneinfoPanelButtonsLayout
        spacing: 0

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        readonly property int itemsHeight: _units.scaled_24

        NpContextMenuButton {
            id: sceneInformationButton
            Layout.fillWidth: true
            Layout.preferredHeight: sceneinfoPanelButtonsLayout.itemsHeight
            text: "Information"
            enabled: true

            topLeftRadius: _units.radiusEight
            topRightRadius: _units.radiusEight

            font: _fonts.label_accent

            onClicked: {
                if (!sceneInfoWindow.visible) {
                    sceneInfoWindow.show();
                }
                sceneinfoPanelComponent.close();
            }

            SIWindow {
                id: sceneInfoWindow

                width: 512
                height: 400

                title: "Scene info"

                contentSource: "../sceneinfo/SceneInfoComponent.qml"
            }
        }

        // NpContextMenuButton {
        //     id: showcaseButton
        //     Layout.fillWidth: true
        //     Layout.preferredHeight: settingsPanelButtonsLayout.itemsHeight
        //     text: "Settings"
        //     enabled: false

        //     font: _fonts.label_accent

        //     onClicked: {
        //         settingsPanelComponent.close()
        //     }
        // }
    }
}
