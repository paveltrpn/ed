// qmllint disable unqualified
// qmllint disable Quick.property-changes-parsed

import QtQuick
import QtQuick.Window

import Tire 1.0

Rectangle {
    id: siEmbeddedWidgetComponent

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    color: "transparent"

    property string title: ""

    property alias contentSource: widgetContentLoader.source

    signal aboutToClose()

    Rectangle {
        id: widgetBody
        anchors.fill: parent
        color: _color.si_background_light
        radius: _units.radiusEight

        border {
            width: _units.scaled_1
            color: _color.si_background_dark
        }

        Rectangle {
            id: titleBar

            anchors {
                top: parent.top
                topMargin: _units.scaled_4
                left: parent.left
                leftMargin: _units.scaled_4
                right: parent.right
                rightMargin: _units.scaled_4
            }

            height: _units.scaled_32
            color: _color.si_background_dark

            topLeftRadius: _units.radiusEight
            topRightRadius: _units.radiusEight

            MouseArea {
                id: headerArea
                anchors.fill: parent
                property bool dragActive
                propagateComposedEvents: true
                onPressed: {
                    siEmbeddedWidgetComponent.startSystemMove();
                }
            }

            Text {
                id: titleText

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                }

                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter

                font: _fonts.label
                color: "white"

                elide: Text.ElideRight

                text: siEmbeddedWidgetComponent.title
            }
        }

        Rectangle {
            id: innerComponentWrappoer

            anchors {
                top: titleBar.bottom
                right: widgetBody.right
                bottom: widgetBody.bottom
                left: widgetBody.left
                margins: _units.scaled_4
            }

            border {
                width: 1
                color: "black"
            }

            Loader {
                id: widgetContentLoader
                anchors {
                    fill: parent
                }
            }
        }
    }
}
