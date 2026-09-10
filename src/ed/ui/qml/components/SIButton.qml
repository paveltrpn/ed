// qmllint disable unqualified
// qmllint disable Quick.property-changes-parsed

import QtQuick
import QtQuick.Shapes.DesignHelpers
import QtQuick.Templates as QuickTemplates
import QtQuick.Controls.impl

import Tire 1.0

QuickTemplates.Button {
    id: control

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    property alias buttonLabel: buttonLabel.text

    states: [
        State {
            when: control.down
            PropertyChanges {
                target: bottomRect
                fillColor: _color.background_overlay_80
            }
            PropertyChanges {
                target: buttonLabel
                color: _color.si_text_light
            }
        },
        State {
            when: control.hovered && !control.checked
            PropertyChanges {
                target: bottomRect
                fillColor: _color.background_overlay_40
            }
        },
        State {
            when: control.checked
            PropertyChanges {
                target: bottomRect
                fillColor: _color.background_overlay_80
            }
            PropertyChanges {
                target: buttonLabel
                color: _color.si_text_light
            }
        }
    ]

    property color textColor: _color.main_contrast

    RectangleShape {
        id: bottomRect
        fillColor: "transparent"//_color.si_background_light
        anchors {
            fill: control
        }
        strokeWidth: _units.scaled_2

        radius: _units.scaled_6
        bevel: true

        Text {
            id: buttonLabel
            anchors {
                fill: parent
                margins: 2
            }

            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter

            color: _color.si_text_dark

            font: _fonts.text_body_accent

            elide: Text.ElideRight
        }
    }
}
