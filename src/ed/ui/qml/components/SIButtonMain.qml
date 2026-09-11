// qmllint disable unqualified
// qmllint disable Quick.property-changes-parsed

import QtQuick
import QtQuick.Templates as QuickTemplates
import QtQuick.Controls.impl

import Tire 1.0

QuickTemplates.Button {
    id: control

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    property color backgroundColor: "#90917e"

    property alias buttonLabel: buttonLabel.text

    property string modeRelatedBgColor

    states: [
        State {
            when: control.down
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
            when: control.hovered && !control.checked
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
            when: control.checked
            PropertyChanges {
                target: control
                backgroundColor: control.modeRelatedBgColor
            }
            PropertyChanges {
                target: buttonLabel
                color: _color.si_text_light
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
                topMargin: 10
                bottomMargin: 6
                leftMargin: 6
                rightMargin: 10
            }
            Rectangle {
                id: pressAreaRect
                color: _color.si_button_press_area

                width: shadowRect.width
                height: shadowRect.height

                x: 4
                y: -4

                Text {
                    id: buttonLabel
                    anchors {
                        fill: parent
                        margins: 2
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
