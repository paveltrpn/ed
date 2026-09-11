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

    property color backgroundColor: "#90917e"

    property alias buttonLabel: buttonLabel.text
    property alias text: textInputComponent.text

    property string modeRelatedBgColor

    // states: [
    //     State {
    //         when: control.down
    //         PropertyChanges {
    //             target: control
    //             backgroundColor: control.modeRelatedBgColor
    //         }
    //         PropertyChanges {
    //             target: pressAreaRect
    //             x: 0
    //             y: 0
    //         }
    //         PropertyChanges {
    //             target: buttonLabel
    //             color: _color.si_text_light
    //         }
    //     },
    //     State {
    //         when: control.hovered && !control.checked
    //         PropertyChanges {
    //             target: control
    //             backgroundColor: control.modeRelatedBgColor
    //         }
    //         PropertyChanges {
    //             target: buttonLabel
    //             color: _color.si_text_light_faded
    //         }
    //     },
    //     State {
    //         when: control.checked
    //         PropertyChanges {
    //             target: control
    //             backgroundColor: control.modeRelatedBgColor
    //         }
    //         PropertyChanges {
    //             target: buttonLabel
    //             color: _color.si_text_light
    //         }
    //     }
    // ]

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

                    color: backgroundColor
                    font: _fonts.label
                    verticalAlignment: Text.AlignVCenter

                    leftPadding: _units.full

                    clip: true

                    Rectangle {
                        z: -1
                        anchors.fill: parent
                        color: _color.si_button_press_area

                        border.color: backgroundColor
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
