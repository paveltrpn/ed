import QtQuick
import QtQuick.Window

import Tire 1.0

import ".."

Window {
    id: colorDialogRoot

    title: "Color"

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    visible: true

    width: 640
    height: 480

    x: parent.x + 100
    y: parent.y + 100

    flags: Qt.Window | Qt.WindowStaysOnTopHint | Qt.FramelessWindowHint

    color: _color.background

    Rectangle {
        id: titleBar

        height: 32

        color: "red"

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
                colorDialogRoot.startSystemMove();
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
                onClicked: colorDialogRoot.close()
            }
        }
    }
}
