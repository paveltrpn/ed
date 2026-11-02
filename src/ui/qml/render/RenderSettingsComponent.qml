// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: renderSettingsItem

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    // color: _color.si_background_light
    color: "transparent"

    signal close

    border {
        width: _units.scaled_1
        color: _color.si_background_dark
    }

    clip: true

    SIButton {
        id: renderButton

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: _units.scaled_4
        }

        height: _units.scaled_32

        buttonLabel: "Render"

        onClicked: {
            Tired.renderRequest.start();
        }
    }
}
