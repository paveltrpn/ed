// qmllint disable unqualified
// qmllint disable Quick.property-changes-parsed

import QtQuick
import QtQuick.Layouts

import Tire 1.0

import "../components"
import "../sceneinfo"
import "../addforms"
import "../settings"

Rectangle {
    id: leftPanelMainComponent

    readonly property var _fonts: Appearence.fonts
    readonly property var _gaps: Appearence.gaps
    readonly property var _radius: Appearence.radius
    readonly property var _color: Appearence.colors

    color: _color.background

    MouseArea {
        id: leftEdgeMoveArea
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
        }
        width: 4
        cursorShape: Qt.SizeHorCursor
        onPressed: {
            MainWindow.resizeWindow(Qt.LeftEdge);
        }
    }

    SIButtonMain {
        id: addObjectButton

        anchors {
            top: parent.top
            topMargin: leftPanelMainComponent._gaps.half
            left: parent.left
            leftMargin: leftPanelMainComponent._gaps.half
            right: parent.right
            rightMargin: leftPanelMainComponent._gaps.half
        }

        height: 56

        buttonLabel: "Add object"

        onClicked: {
            addObjectButton.checked = !addObjectButton.checked;
        }
    }

    AddPrimitivesPopup {
        id: addPrimitivesPopupItem

        anchors {
            top: addObjectButton.bottom
            topMargin: visible ? leftPanelMainComponent._gaps.half : 0
            left: parent.left
            leftMargin: leftPanelMainComponent._gaps.half
            right: parent.right
            rightMargin: leftPanelMainComponent._gaps.half
        }

        visible: addObjectButton.checked
    }

    SIButtonMain {
        id: editObjectButton

        anchors {
            top: addPrimitivesPopupItem.bottom
            topMargin: leftPanelMainComponent._gaps.half
            left: parent.left
            leftMargin: leftPanelMainComponent._gaps.half
            right: parent.right
            rightMargin: leftPanelMainComponent._gaps.half
        }

        height: 56

        buttonLabel: "Edit object"

        onClicked: {
            editObjectButton.checked = !editObjectButton.checked;
        }
    }

    SIButtonMain {
        id: infoButton

        anchors {
            top: editObjectButton.bottom
            topMargin: leftPanelMainComponent._gaps.half
            left: parent.left
            leftMargin: leftPanelMainComponent._gaps.half
            right: parent.right
            rightMargin: leftPanelMainComponent._gaps.half
        }

        height: 56

        buttonLabel: "Info"

        onClicked: {
            infoButton.checked = !infoButton.checked;
        }
    }

    SceneInfoComponent {
        id: sceneInfoWidget
        anchors {
            top: infoButton.bottom
            topMargin: _gaps.half
            left: parent.left
            leftMargin: leftPanelMainComponent._gaps.half
            right: parent.right
            rightMargin: leftPanelMainComponent._gaps.half
        }

        visible: infoButton.checked
    }

    SIButtonMain {
        id: settingsButton

        anchors {
            bottom: parent.bottom
            left: parent.left
            leftMargin: leftPanelMainComponent._gaps.half
            right: parent.right
            rightMargin: leftPanelMainComponent._gaps.half
        }

        height: 56

        buttonLabel: "Settings"

        onClicked: {
            settingsButton.checked = !settingsButton.checked;
        }
    }

    SettingsComponent {
        id: settingsWidget
        anchors {
            bottom: settingsButton.top
            bottomMargin: _gaps.half
            left: parent.left
            leftMargin: leftPanelMainComponent._gaps.half
            right: parent.right
            rightMargin: leftPanelMainComponent._gaps.half
        }

        visible: settingsButton.checked
    }
}
