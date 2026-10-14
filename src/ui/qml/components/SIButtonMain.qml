// qmllint disable unqualified
// qmllint disable Quick.property-changes-parsed

import QtQuick
import QtQuick.Templates as QuickTemplates
import QtQuick.Controls.impl

import Tire 1.0

QuickTemplates.Button {
    id: control

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    property color backgroundColor: _color.si_button_underlying

    property alias buttonLabel: buttonLabel.text

    property string modeRelatedBgColor

    // "0" - from left to right.
    // "1" - from right to left.
    property int pressAreaAlignment: 0

    height: _units.scaled_42

    opacity: enabled ? 1.0 : 0.64

    states: [
        State {
            when: enabled && control.down
            PropertyChanges {
                target: control
                backgroundColor: control.modeRelatedBgColor
            }
            PropertyChanges {
                target: pressAreaRect
                x: 0
                y: 0
            }
            PropertyChanges {
                target: buttonLabel
                color: _color.si_text_light
            }
        },
        State {
            when: enabled && (control.hovered && !control.checked)
            PropertyChanges {
                target: control
                backgroundColor: control.modeRelatedBgColor
            }
            PropertyChanges {
                target: buttonLabel
                color: _color.si_text_light_faded
            }
        },
        State {
            when: enabled && control.checked
            PropertyChanges {
                target: control
                backgroundColor: control.modeRelatedBgColor
            }
            PropertyChanges {
                target: buttonLabel
                color: _color.si_text_light
            }
            PropertyChanges {
                target: pressAreaRect
                x: 0
                y: 0
            }
        }
    ]

    Rectangle {
        id: bottomRect
        color: backgroundColor
        anchors {
            fill: control
        }

        Rectangle {
            id: shadowRect
            color: _color.si_button_shadow
            anchors {
                fill: parent
                topMargin: _units.scaled_10
                bottomMargin: _units.scaled_6
                leftMargin: (pressAreaAlignment === 0) ? _units.scaled_6 : _units.scaled_10
                rightMargin: (pressAreaAlignment === 0) ? _units.scaled_10 : _units.scaled_6
            }
            Rectangle {
                id: pressAreaRect
                color: _color.si_button_press_area

                width: shadowRect.width
                height: shadowRect.height

                x: pressAreaAlignment === 0 ? _units.scaled_4 : -_units.scaled_4
                y: -_units.scaled_4

                Text {
                    id: buttonLabel
                    anchors {
                        fill: parent
                        margins: _units.scaled_2
                        topMargin: _units.scaled_6
                    }

                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter

                    color: _color.si_text_light_faded

                    font: _fonts.text_body_accent

                    elide: Text.ElideRight
                }
            }
        }
    }
}
