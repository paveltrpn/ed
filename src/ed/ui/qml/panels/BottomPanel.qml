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

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    color: _color.background

    // bottomLeftRadius:  _units.radiusEight
    // bottomRightRadius: _units.radiusEight

    // MouseArea {
    //     id: bottomEdgeMoveArea
    //     anchors {
    //         left: parent.left
    //         right: parent.right
    //         bottom: parent.bottom
    //     }
    //     height: _units.scaled_4
    //     cursorShape: Qt.SizeVerCursor
    //     onPressed: {
    //         MainWindow.resizeWindow(Qt.BottomEdge);
    //     }
    // }

    Rectangle {
        id: bottomPanelMainComponentWrapper
        anchors.fill: parent
        color: _color.si_button_shadow
        bottomLeftRadius: _units.radiusEight
        bottomRightRadius: _units.radiusEight
    }
}
