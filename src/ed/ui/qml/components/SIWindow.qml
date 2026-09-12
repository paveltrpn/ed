import QtQuick
import QtQuick.Window

import Tire 1.0

import ".."

Window {
    id: siWindowComponent

    title: "Color"

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    visible: true

    flags: Qt.Window | Qt.WindowStaysOnTopHint | Qt.FramelessWindowHint

    color: _color.si_background_light

    Rectangle {
        id: titleBar

        height: 32

        color: _color.si_background_dark

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        MouseArea {
            id: headerArea
            anchors.fill: parent

            property point lastMousePos: Qt.point(0, 0)

            property bool dragActive
            propagateComposedEvents: true

            onPressed: {
                siWindowComponent.startSystemMove();
            }
        }

        Row {
            anchors.right: parent.right
            anchors.rightMargin: 5
            spacing: 5
            height: parent.height

            NpButton {
                text: "X"
                width: 32
                height: 32
                onClicked: siWindowComponent.close()
            }
        }
    }
}

