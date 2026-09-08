// qmllint disable unqualified
// qmllint disable Quick.property-changes-parsed

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Tire 1.0

import "../components"
import "../components/filedialog"

Rectangle {
    id: bottomPanelMainComponent

    readonly property var _fonts: Appearence.fonts
    readonly property var _gaps: Appearence.gaps
    readonly property var _radius: Appearence.radius
    readonly property var _color: Appearence.colors

    color: _color.background

    bottomLeftRadius: _radius.half
    bottomRightRadius: _radius.half

    MouseArea {
        id: bottomEdgeMoveArea
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: 4
        cursorShape: Qt.SizeVerCursor
        onPressed: {
            MainWindow.resizeWindow(Qt.BottomEdge);
        }
    }

    Rectangle {
        id: bottomPanelMainComponentWrapper
        anchors.fill: parent
        color: _color.background_overlay_60
        bottomLeftRadius: _radius.half
        bottomRightRadius: _radius.half
    }
}
