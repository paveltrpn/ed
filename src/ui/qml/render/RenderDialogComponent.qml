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
            top: parent.top
        }
    }

    RenderSettingsComponent {
        id: settingsArea

        anchors {
            right: parent.right
            left: imageArea.right
            leftMargin: _units.scaled_4
            top: parent.top
            bottom: processLogArea.top
            bottomMargin: _units.scaled_4
        }
    }

    RenderProcessLogComponent {
        id: processLogArea

        anchors {
            top: imageArea.bottom
            topMargin: _units.scaled_4
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
    }
}
