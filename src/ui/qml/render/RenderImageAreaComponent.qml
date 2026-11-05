// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: renderImageAreaItem

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    border {
        width: _units.scaled_1
        color: _color.si_background_dark
    }

    color: _color.si_background_light_faded

    RenderImageItem {
        anchors {
            fill: parent;
            margins: _units.scaled_4
        }

        image: Tired.renderRequest.renderedImage
    }
}
