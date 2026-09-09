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

    readonly property string txtColorOff: "#5d5d5b"
    readonly property string txtColorOn: "white"

    property alias buttonLabel: buttonLabel.text

    states: [
        State {
            when: control.down
            PropertyChanges {
                target: control
                backgroundColor: _color.mode_model
            }
            PropertyChanges {
                target: pressAreaRect
                x: 0
                y: 0
            }
            PropertyChanges {
                target: buttonLabel
                color: _color.text_active
            }
        },
        State {
            when: control.hovered && !control.checked
            PropertyChanges {
                target: control
                backgroundColor: _color.mode_model
            }
            PropertyChanges {
                target: buttonLabel
                color: _color.text_inactive
            }
        },
        State {
            when: control.checked
            PropertyChanges {
                target: control
                backgroundColor: _color.mode_model
            }
            PropertyChanges {
                target: buttonLabel
                color: _color.text_active
            }
        }
    ]

    property color textColor: _color.main_contrast

    Rectangle {
        id: bottomRect
        color: backgroundColor
        anchors {
            fill: control
        }

        Rectangle {
            id: shadowRect
            color: _color.button_shadow
            anchors {
                fill: parent
                topMargin: 10
                bottomMargin: 6
                leftMargin: 6
                rightMargin: 10
            }
            Rectangle {
                id: pressAreaRect
                color: _color.button_press_area

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

                    color: txtColorOff
                    font: _fonts.text_body_accent

                    elide: Text.ElideRight
                }
            }
        }
    }
}
