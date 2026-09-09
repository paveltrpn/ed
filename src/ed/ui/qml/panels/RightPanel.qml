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

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    color: _color.background

    MouseArea {
        id: rightEdgeMoveArea
        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
        }
        width: _units.scaled_6
        cursorShape: Qt.SizeHorCursor
        onPressed: {
            MainWindow.resizeWindow(Qt.RightEdge);
        }
    }

    Rectangle {
        id: rightPanelMainComponentWrapper
        anchors.fill: parent
        color: _color.button_shadow
    }
}
