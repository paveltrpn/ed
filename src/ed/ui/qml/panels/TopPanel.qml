// qmllint disable unqualified
// qmllint disable Quick.property-changes-parsed

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Tire 1.0

import "../components"
import "../components/filedialog"

Rectangle {
    id: topPanelMainComponent

    readonly property var _fonts: Appearence.fonts
    readonly property var _gaps: Appearence.gaps
    readonly property var _radius: Appearence.radius
    readonly property var _color: Appearence.colors

    color: _color.background

    topLeftRadius: _radius.half
    topRightRadius: _radius.half

    MouseArea {
        id: topEdgeMoveArea
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: 4
        cursorShape: Qt.SizeVerCursor
        onPressed: {
            MainWindow.resizeWindow(Qt.TopEdge);
        }
    }

    MouseArea {
        id: windowMoveArea
        anchors {
            left: parent.left
            right: parent.right
            top: topEdgeMoveArea.bottom
            bottom: parent.bottom
        }
        onPressed: {
            MainWindow.moveWindow();
        }
    }

    NpFileDialog {
        id: fileDialog
    }

    Rectangle {
        id: topPanelMainComponentWrapper
        anchors.fill: parent
        color: _color.background_overlay_60
        topLeftRadius: _radius.half
        topRightRadius: _radius.half

        Item {
            id: menuBarWrapper

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }

            height: 24

            property bool isMenuActive: false
            property var activeMenu: null

            function openMenu(menu) {
                if (menuBarWrapper.activeMenu && menuBarWrapper.activeMenu !== menu) {
                    menuBarWrapper.activeMenu.close();
                }
                menu.open();
                menuBarWrapper.isMenuActive = true;
                menuBarWrapper.activeMenu = menu;
            }

            function closeActiveMenu() {
                if (menuBarWrapper.activeMenu) {
                    menuBarWrapper.activeMenu.close();
                    menuBarWrapper.activeMenu = null;
                }
                menuBarWrapper.isMenuActive = false;
            }

            RowLayout {
                id: menuBarButtonsLayout

                spacing: 0

                anchors {
                    left: parent.left
                    leftMargin: _gaps.full
                    top: parent.top
                    bottom: parent.bottom
                    right: mainWindowDecorationWrapper.left
                }

                NpMenuBarButton {
                    id: fileMenuButton

                    Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

                    Layout.preferredHeight: 24

                    text: "File"
                    font: _fonts.label_accent

                    onClicked: {
                        if (menuBarWrapper.activeMenu === fileMenu) {
                            menuBarWrapper.closeActiveMenu();
                        } else {
                            menuBarWrapper.openMenu(fileMenu);
                        }
                    }

                    onHoveredChanged: {
                        if (hovered && menuBarWrapper.isMenuActive && menuBarWrapper.activeMenu !== fileMenu) {
                            menuBarWrapper.openMenu(fileMenu);
                        }
                    }

                    backgroundColor: "transparent"

                    Popup {
                        id: fileMenu

                        y: parent.height

                        width: 256
                        height: fileMenuColumn.implicitHeight

                        padding: 0

                        popupType: Popup.Native
                        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

                        Component.onCompleted: {
                            forceActiveFocus(); // Forces focus and layout resolution
                            // Optional: you can open it in the next event loop tick
                            //Qt.callLater(fileMenu.open)
                        }

                        onVisibleChanged: {
                            if (!visible && menuBarWrapper.activeMenu === fileMenu) {
                                menuBarWrapper.closeActiveMenu();
                            }
                        }

                        contentItem: Rectangle {
                            width: parent.width
                            color: _color.background
                            Column {
                                id: fileMenuColumn
                                width: parent.width
                                NpContextMenuButton {
                                    width: parent.width
                                    height: 24
                                    text: "Open"
                                    icon.source: "image://TiredImageProvider/folder.svg"
                                    font: _fonts.label_accent
                                    onClicked: {
                                        fileMenu.close();
                                        fileDialog.show();
                                    }
                                }
                                NpContextMenuButton {
                                    width: parent.width
                                    height: 24
                                    text: "Save"
                                    icon.source: "image://TiredImageProvider/save.svg"
                                    font: _fonts.label_accent
                                    onClicked: {
                                        fileMenu.close();
                                        fileDialog.show();
                                    }
                                }
                                NpContextMenuButton {
                                    width: parent.width
                                    height: 24
                                    text: "Exit"
                                    icon.source: "image://TiredImageProvider/door-open.svg"
                                    font: _fonts.label_accent
                                    onClicked: {
                                        fileMenu.close();
                                        MainWindow.quitApplication();
                                    }
                                }
                            }
                        }
                    }
                }

                NpMenuBarButton {
                    id: helpMenuButton

                    Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

                    Layout.preferredHeight: 24

                    text: "Help"
                    font: _fonts.label_accent

                    onClicked: {
                        if (menuBarWrapper.activeMenu === helpMenu) {
                            menuBarWrapper.closeActiveMenu();
                        } else {
                            menuBarWrapper.openMenu(helpMenu);
                        }
                    }

                    onHoveredChanged: {
                        if (hovered && menuBarWrapper.isMenuActive && menuBarWrapper.activeMenu !== helpMenu) {
                            menuBarWrapper.openMenu(helpMenu);
                        }
                    }

                    backgroundColor: "transparent"

                    Popup {
                        id: helpMenu

                        y: parent.height

                        width: 256
                        height: helpMenuColumn.implicitHeight

                        padding: 0

                        popupType: Popup.Window

                        contentItem: Rectangle {
                            width: parent.width
                            color: _color.background
                            Column {
                                id: helpMenuColumn
                                width: parent.width
                                NpContextMenuButton {
                                    width: parent.width
                                    height: 24
                                    text: "About..."
                                    icon.source: "image://TiredImageProvider/circle-exclamation.svg"
                                    font: _fonts.label_accent
                                    onClicked: {
                                        helpMenu.close();
                                    }
                                }
                            }
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                }
            }

            Item {
                id: mainWindowDecorationWrapper

                anchors {
                    right: parent.right
                    rightMargin: _gaps.full
                    top: parent.top
                    bottom: parent.bottom
                }

                width: 128

                NpButton {
                    id: maximizeWindowButton

                    anchors {
                        right: foldWindowButton.left
                        rightMargin: 4
                        top: parent.top
                    }

                    width: 24
                    height: 24

                    icon.source: "image://TiredImageProvider/arrow-up-right-from-square.svg"
                    onClicked: {}
                }

                NpButton {
                    id: foldWindowButton

                    anchors {
                        right: closeAppButton.left
                        rightMargin: 4
                        top: parent.top
                    }

                    width: 24
                    height: 24

                    icon.source: "image://TiredImageProvider/square-minus.svg"
                    onClicked: {}
                }

                NpButton {
                    id: closeAppButton

                    anchors {
                        right: parent.right
                        top: parent.top
                    }

                    width: 24
                    height: 24

                    icon.source: "image://TiredImageProvider/xmark-large.svg"
                    onClicked: {
                        MainWindow.quitApplication();
                    }
                }
            }
        }

        Item {
            id: mainTopPanel

            anchors {
                left: parent.left
                right: parent.right
                top: menuBarWrapper.bottom
                topMargin: _gaps.half
            }

            height: 32

            RowLayout {
                id: topPanelButtonsLayout

                spacing: _gaps.half

                anchors {
                    fill: parent
                }

                Item {
                    id: spacerLeft
                    Layout.preferredWidth: _gaps.half
                }

                NpButton {
                    id: gizmoMoveModeBtn
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                    icon.source: "image://TiredImageProvider/arrow-all-330.svg"
                    onClicked: {}
                }

                NpButton {
                    id: gizmoRotateModeBtn
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                    icon.source: "image://TiredImageProvider/arrow-spin.svg"
                    onClicked: {}
                }

                NpButton {
                    id: gizmoScaleModeBtn
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                    icon.source: "image://TiredImageProvider/arrow-corner-278.svg"
                    onClicked: {}
                }

                Item {
                    id: spacer2
                    Layout.preferredWidth: _gaps.full
                }

                Item {
                    id: gizmoCoordinateSystemModeBtnsWrapper

                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

                    Layout.preferredWidth: gizmoLocalModeBtn.width + gizmoGlobalModeBtn.width + _gaps.half
                    // implicitWidth: gizmoLocalModeBtn.width + gizmoGlobalModeBtn.width

                    NpButton {
                        id: gizmoLocalModeBtn

                        anchors {
                            verticalCenter: parent.verticalCenter
                            left: parent.left
                        }

                        icon.source: "image://TiredImageProvider/cube-alt.svg"
                        onClicked: {}
                    }

                    NpButton {
                        id: gizmoGlobalModeBtn

                        anchors {
                            verticalCenter: parent.verticalCenter
                            right: parent.right
                        }

                        Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                        icon.source: "image://TiredImageProvider/cube_01.svg"
                        onClicked: {}
                    }
                }

                Item {
                    id: buttonsSpacer
                    Layout.fillWidth: true
                }

                Text {
                    id: sclLabel

                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

                    text: {
                        const pos = Tired.inputHandler.mousePos;
                        return `X: ${pos.x} Y: ${pos.y}`;
                    }

                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter

                    padding: 8

                    color: _color.additional_contrast_60
                    font: _fonts.label
                }

                Item {
                    id: spacerRight
                    Layout.preferredWidth: _gaps.half
                }
            }
        }
    }
}
