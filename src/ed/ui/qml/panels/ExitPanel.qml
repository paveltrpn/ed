// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: exitPanelComponent

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    color: _color.si_background_light
    radius: _units.radiusEight
    height: visible ? _units.scaled_64 : 0

    signal accept()
    signal decline()

    Behavior on height {
        NumberAnimation {
            duration: 200
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: _units.scaled_8

        Item {}

        SIButton {
            id: declineButton

            Layout.fillWidth: true
            Layout.preferredHeight: _units.scaled_32

            buttonLabel: "No"

            onClicked: {
                decline()
            }
        }

        SIButton {
            id: acceptButton

            Layout.fillWidth: true
            Layout.preferredHeight: _units.scaled_32

            buttonLabel: "Yes"

            onClicked: {
                accept()
            }
        }

        Item {}
    }
}
