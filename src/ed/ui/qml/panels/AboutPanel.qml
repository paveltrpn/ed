// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: aboutPanelComponent

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    color: _color.si_background_light
    radius: _units.radiusEight
    height: visible ? aboutPanelPopupComponent.height : 0

    signal accept
    signal decline

    signal close

    Behavior on height {
        NumberAnimation {
            duration: 200
        }
    }
    Item {
        id: aboutPanelPopupComponent

        readonly property var _color: Appearence.colors
        readonly property var _fonts: Appearence.fonts
        readonly property var _units: Appearence.units

        width: parent.width
        height: aboutPanelButtonsLayout.implicitHeight

        Behavior on height {
            NumberAnimation {
                duration: 200
            }
        }

        ColumnLayout {
            id: aboutPanelButtonsLayout
            spacing: 0

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }

            readonly property int itemsHeight: _units.scaled_32

            NpContextMenuButton {
                id: aboutButton
                Layout.fillWidth: true
                Layout.preferredHeight: aboutPanelButtonsLayout.itemsHeight
                text: "About"
                enabled: true

                topLeftRadius: _units.radiusEight
                topRightRadius: _units.radiusEight

                font: _fonts.label_accent

                onClicked: {
                    aboutPanelComponent.close()
                }
            }

            NpContextMenuButton {
                id: showcaseButton
                Layout.fillWidth: true
                Layout.preferredHeight: aboutPanelButtonsLayout.itemsHeight
                text: "Showcase"
                enabled: false

                font: _fonts.label_accent
            }

            NpContextMenuButton {
                id: vulkanInfoButton
                Layout.fillWidth: true
                Layout.preferredHeight: aboutPanelButtonsLayout.itemsHeight
                text: "Vulkan info"
                enabled: false

                font: _fonts.label_accent
            }
        }
    }
}
