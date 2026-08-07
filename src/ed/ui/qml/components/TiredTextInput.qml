import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0

TextInput {
    id: textInputComponent

    readonly property var _fonts: Appearence.fonts
    readonly property var _gaps: Appearence.gaps
    readonly property var _radius: Appearence.radius
    readonly property var _color: Appearence.colors

    height: 24
    color: _color.main_contrast
    font: _fonts.label
    verticalAlignment: Text.AlignVCenter

    leftPadding: _gaps.full

    clip: true

    Rectangle {
        z: -1
        anchors.fill: parent
        color: inputArea.containsMouse ? _color.background_overlay_light_40 : _color.background_overlay_40

        border.color: _color.main_contrast_20
        border.width: 2
        radius: _radius.quarter

        MouseArea {
            id: inputArea
            hoverEnabled: true
            anchors.fill: parent
        }
    }
}
