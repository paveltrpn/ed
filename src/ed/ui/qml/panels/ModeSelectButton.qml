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

    property alias buttonLabel: buttonLabel.text

    implicitWidth: buttonRect.implicitWidth

    states: [
        State {
            when: control.down
            PropertyChanges {
                target: buttonLabel
                color: _color.si_text_light
            }
        },
        State {
            when: control.hovered && !control.checked
            PropertyChanges {
                target: buttonLabel
                color: _color.si_text_light_faded
            }
        },
        State {
            when: control.checked

            PropertyChanges {
                target: buttonLabel
                color: _color.si_text_light
            }
        }
    ]

    property color textColor: _color.main_contrast

    Item {
        id: buttonRect

        anchors {
            top: parent.top
            bottom: parent.bottom
        }

        implicitWidth: buttonLabel.implicitWidth

        Text {
            id: buttonLabel

            anchors {
                top: parent.top
                bottom: parent.bottom
            }

            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter

            color: _color.si_text_dark

            font: _fonts.title_accent

            elide: Text.ElideRight
        }
    }
}
