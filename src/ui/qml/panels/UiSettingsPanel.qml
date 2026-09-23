// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: uiSettingsPanelComponent

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    color: _color.si_background_light
    radius: _units.radiusEight
    height: visible ? aboutPanelButtonsLayout.implicitHeight : 0

    signal close

    Behavior on height {
        NumberAnimation {
            duration: 50
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

        readonly property int itemsHeight: _units.scaled_24

        NpContextMenuButton {
            id: restorePanelsSizeButton
            Layout.fillWidth: true
            Layout.preferredHeight: aboutPanelButtonsLayout.itemsHeight
            text: "Restore panels size"
            enabled: true

            font: _fonts.label_accent

            onClicked: {
                Tired.restorePanelsSize()
                uiSettingsPanelComponent.close()
            }
        }
    }
}
