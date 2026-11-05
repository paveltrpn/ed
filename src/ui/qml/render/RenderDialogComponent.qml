// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: renderComponent

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    color: _color.si_background_light
    radius: _units.radiusEight

    RenderImageAreaComponent {
        id: imageArea

        anchors {
            left: parent.left
            leftMargin: _units.scaled_4
            top: parent.top
            topMargin: _units.scaled_4
            bottom: parent.bottom
            bottomMargin: _units.scaled_4
        }

        width: parent.width * 0.75
    }

    RenderSettingsComponent {
        id: settingsArea

        anchors {
            right: parent.right
            rightMargin: _units.scaled_4
            left: imageArea.right
            leftMargin: _units.scaled_4
            top: parent.top
            topMargin: _units.scaled_4
            bottom: parent.bottom
            bottomMargin: _units.scaled_4
        }
    }
}
