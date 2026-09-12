// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: settingsPanelComponent

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    color: _color.si_background_light
    radius: _units.radiusEight
    height: visible ? settingsPanelButtonsLayout.implicitHeight : 0

    signal close

    Behavior on height {
        NumberAnimation {
            duration: 50
        }
    }

    ColumnLayout {
        id: settingsPanelButtonsLayout
        spacing: 0

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        readonly property int itemsHeight: _units.scaled_32

        NpContextMenuButton {
            id: serviceObjectsSettingsButton
            Layout.fillWidth: true
            Layout.preferredHeight: settingsPanelButtonsLayout.itemsHeight
            text: "Service objects"
            enabled: true

            topLeftRadius: _units.radiusEight
            topRightRadius: _units.radiusEight

            font: _fonts.label_accent

            onClicked: {
                if (!settingsWindow.visible) {
                    settingsWindow.show();
                }

                settingsPanelComponent.close()
            }

            SIWindow {
                id: settingsWindow

                width: 410
                height: 320

                contentSource: "../settings/SettingsComponent.qml"
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

