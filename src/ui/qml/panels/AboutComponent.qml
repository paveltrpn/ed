// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: aboutComponent

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    color: _color.si_background_light

    border {
        width: _units.scaled_1
        color: _color.si_background_dark
    }

    SIButton {
        id: closeButton

        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: _units.scaled_4
        }

        height: _units.scaled_32
        width: _units.scaled_64

        buttonLabel: "Close"

        onClicked: {
            siEmbeddedWidgetComponent.aboutToClose();
        }
    }
}
