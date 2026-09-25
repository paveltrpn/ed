// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: selectedInfoComponent

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    bottomLeftRadius: _units.radiusEight
    bottomRightRadius: _units.radiusEight

    color: _color.si_background_light

    // implicitHeight: selectedInfoLayout.implicitHeight + _units.full

    Item {
        id: objectInfoArea

        anchors {
            fill: parent
        }

        states: [
            State {
                name: "no_object_selected"
                when: !Tired.scenegraph.scene.isAnyObjectSelected
                PropertyChanges {
                    noObjectSelectedDummy.visible: true
                    someObjectSelectedWrapper.visible: false
                }
            },
            State {
                name: "some_object_selected"
                when: Tired.scenegraph.scene.isAnyObjectSelected
                PropertyChanges {
                    noObjectSelectedDummy.visible: false
                    someObjectSelectedWrapper.visible: true
                }
            }
        ]

        Text {
            id: noObjectSelectedDummy

            anchors {
                fill: parent
            }

            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter

            font: _fonts.text_body_accent

            color: _color.si_background_dark

            text: "No objects selected"
        }

        Rectangle {
            id: someObjectSelectedWrapper

            anchors {
                centerIn: parent
            }

            width: 100
            height: 100

            color: "red"
        }
    }
}
