// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: renderProcessLogItem

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
}

