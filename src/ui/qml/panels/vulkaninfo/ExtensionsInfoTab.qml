// qmllint disable unqualified

import QtQuick

import Tire 1.0

// Placeholder tab: device extensions data.
Rectangle {
    id: extensionsInfoTab

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    bottomLeftRadius: _units.radiusEight
    bottomRightRadius: _units.radiusEight

    color: _color.si_background_light

    Text {
        id: placeholderText

        anchors.centerIn: parent

        color: _color.si_text_light_faded

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter

        font: _fonts.text_body_accent

        text: "EXTENSIONS INFO:"
    }
}
