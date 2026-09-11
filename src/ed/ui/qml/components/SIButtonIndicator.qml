// qmllint disable unqualified
// qmllint disable Quick.property-changes-parsed

import QtQuick
import QtQuick.Shapes.DesignHelpers
import QtQuick.Shapes

import Tire 1.0

Item {
    id: control

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    property alias buttonLabel: buttonLabel.text
    property alias text: textInputComponent.text

    property string modeRelatedBgColor

    // "0" - from left to right.
    // "1" - from right to left.
    property int pressAreaAlignment: 0

    states: [
        State {
            when: indicatorArea.containsMouse || textInputComponent.focus
            PropertyChanges {
                target: buttonLabel
                color: _color.si_text_light
            }
        },
        State {
            when: !indicatorArea.containsMouse
            PropertyChanges {
                target: buttonLabel
                color: _color.si_text_light_faded
            }
        }
    ]

    Rectangle {
        id: bottomRect
        color: _color.si_button_underlying
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
                leftMargin: (pressAreaAlignment === 0) ? 6 : 10
                rightMargin: (pressAreaAlignment === 0) ? 10 : 6
            }
            Rectangle {
                id: pressAreaRect
                color: _color.si_button_press_area

                width: shadowRect.width
                height: shadowRect.height

                x: (pressAreaAlignment === 0) ? 4 : -4
                y: -4

                MouseArea {
                    id: indicatorArea
                    anchors.fill: parent
                    hoverEnabled: true

                    onClicked: {
                        textInputComponent.focus = true
                    }
                }

                Text {
                    id: buttonLabel
                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                    }

                    height: parent.height / 2

                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter

                    color: _color.si_text_light_faded

                    font: _fonts.text_body_accent

                    elide: Text.ElideRight
                }

                TextInput {
                    id: textInputComponent

                    anchors {
                        left: parent.left
                        leftMargin: _units.scaled_16
                        right: parent.right
                        rightMargin: _units.scaled_16
                        bottom: parent.bottom
                        top: parent.top
                        topMargin: _units.scaled_20
                    }

                    color: _color.si_button_underlying
                    font: _fonts.label
                    verticalAlignment: Text.AlignVCenter

                    leftPadding: _units.full

                    clip: true

                    Rectangle {
                        id: borderRect
                        z: -1
                        anchors {
                            fill: parent
                            bottomMargin: -borderRect.border.width
                        }
                        color: _color.si_button_press_area

                        border.color: _color.si_button_underlying
                        border.width: _units.scaled_1
                        radius: 0

                        MouseArea {
                            id: inputArea
                            hoverEnabled: true
                            anchors.fill: parent
                        }
                    }
                }
            }
        }
    }
}
