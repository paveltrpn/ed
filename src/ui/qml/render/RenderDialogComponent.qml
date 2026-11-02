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
    height: visible ? settingsPanelButtonsLayout.implicitHeight : 0

    signal close

    RenderImageAreaComponent {
        id:
    }
}
