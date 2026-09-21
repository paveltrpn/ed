// qmllint disable unqualified
// qmllint disable Quick.property-changes-parsed

import QtQuick
import QtQuick.Shapes.DesignHelpers
import QtQuick.Templates as QuickTemplates
import QtQuick.Controls.impl

import Tire 1.0

QuickTemplates.Button {
    id: control

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    property alias buttonLabel: buttonLabel.text

    states: [
        State {
            when: control.down
            PropertyChanges {
                target: bottomRect
                color: _color.background_overlay_80
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
                color: _color.background_overlay_40
            }
        },
        State {
            when: control.checked
            PropertyChanges {
                target: bottomRect
                color: _color.si_button_bg_blue
            }
            PropertyChanges {
                target: buttonLabel
                color: _color.si_text_light
            }
        }
    ]

    property color textColor: _color.main_contrast

    Rectangle {
        id: bottomRect
        color: "transparent"//_color.si_background_light

        anchors {
            fill: control
        }

        border {
            width: _units.scaled_2
            color: _color.si_background_dark
        }

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
