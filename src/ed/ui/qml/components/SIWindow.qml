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

    flags: Qt.Window | Qt.WindowStaysOnTopHint | Qt.FramelessWindowHint
    color: "transparent"

    onVisibleChanged: {
        if (siWindowComponent.visible) {
            const mainWndCenter = MainWindow.mainWindowCenter();
            siWindowComponent.x = mainWndCenter.x - siWindowComponent.width / 2;
            siWindowComponent.y = mainWndCenter.y - siWindowComponent.height / 2;
        } else {}
    }

    MouseArea {
        id: topEdgeMoveArea
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: _units.scaled_4
        cursorShape: Qt.SizeVerCursor
        onPressed: {
            siWindowComponent.startSystemResize(Qt.TopEdge);
        }
    }

    MouseArea {
        id: bottomEdgeMoveArea
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: _units.scaled_4
        cursorShape: Qt.SizeVerCursor
        onPressed: {
            siWindowComponent.startSystemResize(Qt.BottomEdge);
        }
    }

    MouseArea {
        id: rightEdgeMoveArea
        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
        }
        width: _units.scaled_4
        cursorShape: Qt.SizeHorCursor
        onPressed: {
            siWindowComponent.startSystemResize(Qt.RightEdge);
        }
    }

    MouseArea {
        id: leftEdgeMoveArea
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
        }
        width: _units.scaled_4
        cursorShape: Qt.SizeHorCursor
        onPressed: {
            siWindowComponent.startSystemResize(Qt.LeftEdge);
        }
    }

    Rectangle {
        id: windowBody
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
                left: parent.left
                right: parent.right
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
}
