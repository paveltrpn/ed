// qmllint disable unqualified
// qmllint disable Quick.property-changes-parsed

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

    onVisibleChanged: {
        if (siWindowComponent.visible) {
            const mainWndCenter = MainWindow.mainWindowCenter()
            siWindowComponent.x = mainWndCenter.x - siWindowComponent.width / 2
            siWindowComponent.y = mainWndCenter.y - siWindowComponent.height / 2
        } else {
        }
    }

    Rectangle {
        id: titleBar

        height: _units.scaled_32

        color: _color.si_background_dark

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        MouseArea {
            id: headerArea
            anchors.fill: parent
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
                icon.source: "image://TiredImageProvider/filled-triangle-down.svg"
                width: _units.scaled_24
                height: _units.scaled_24
                onClicked: siWindowComponent.close()
            }
        }
    }
}
