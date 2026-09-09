import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0

TextInput {
    id: textInputComponent

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    height: _units.scaled_24
    color: _color.text_contrast
    font: _fonts.label
    verticalAlignment: Text.AlignVCenter

    leftPadding: _units.full

    clip: true

    Rectangle {
        z: -1
        anchors.fill: parent
        color: inputArea.containsMouse ? _color.background_overlay_light_40 : _color.popup_background

        border.color: _color.popup_header
        border.width: _units.scaled_2
        radius: _units.radiusEight

        MouseArea {
            id: inputArea
            hoverEnabled: true
            anchors.fill: parent
        }
    }
}
