// qmllint disable unqualified
// qmllint disable Quick.property-changes-parsed

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Tire 1.0

import "../components"
import "../components/filedialog"

Rectangle {
    id: rightPanelMainComponent

    readonly property var _fonts: Appearence.fonts
    readonly property var _gaps: Appearence.gaps
    readonly property var _radius: Appearence.radius
    readonly property var _color: Appearence.colors

    color: _color.background

    MouseArea {
        id: rightEdgeMoveArea
        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
        }
        width: 6
        cursorShape: Qt.SizeHorCursor
        onPressed: {
            MainWindow.resizeWindow(Qt.RightEdge);
        }
    }

    Rectangle {
        id: rightPanelMainComponentWrapper
        anchors.fill: parent
        color: _color.background_overlay_60
    }
}
