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

    topLeftRadius: _radius.eight
    topRightRadius: _radius.eight

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
        color: _color.button_shadow
        topLeftRadius: _radius.eight
        topRightRadius: _radius.eight

        // Item {
        //     id: menuBarWrapper

        //     anchors {
        //         left: parent.left
        //         right: parent.right
        //         top: parent.top
        //     }

        //     height: 24

        //     property bool isMenuActive: false
        //     property var activeMenu: null

        //     function openMenu(menu) {
        //         if (menuBarWrapper.activeMenu && menuBarWrapper.activeMenu !== menu) {
        //             menuBarWrapper.activeMenu.close();
        //         }
        //         menu.open();
        //         menuBarWrapper.isMenuActive = true;
        //         menuBarWrapper.activeMenu = menu;
        //     }

        //     function closeActiveMenu() {
        //         if (menuBarWrapper.activeMenu) {
        //             menuBarWrapper.activeMenu.close();
        //             menuBarWrapper.activeMenu = null;
        //         }
        //         menuBarWrapper.isMenuActive = false;
        //     }

        //     RowLayout {
        //         id: menuBarButtonsLayout

        //         spacing: 0

        //         anchors {
        //             left: parent.left
        //             leftMargin: _gaps.full
        //             top: parent.top
        //             bottom: parent.bottom
        //             right: mainWindowDecorationWrapper.left
        //         }

        //         NpMenuBarButton {
        //             id: fileMenuButton

        //             Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

        //             Layout.preferredHeight: 24

        //             text: "File"
        //             font: _fonts.label_accent

        //             onClicked: {
        //                 if (menuBarWrapper.activeMenu === fileMenu) {
        //                     menuBarWrapper.closeActiveMenu();
        //                 } else {
        //                     menuBarWrapper.openMenu(fileMenu);
        //                 }
        //             }

        //             onHoveredChanged: {
        //                 if (hovered && menuBarWrapper.isMenuActive && menuBarWrapper.activeMenu !== fileMenu) {
        //                     menuBarWrapper.openMenu(fileMenu);
        //                 }
        //             }

        //             backgroundColor: "transparent"

        //             Popup {
        //                 id: fileMenu

        //                 y: parent.height

        //                 width: 256
        //                 height: fileMenuColumn.implicitHeight

        //                 popupType: Popup.Window
        //                 closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        //                 padding: 0

        //                 Component.onCompleted: {
        //                     forceActiveFocus();
        //                 }

        //                 onVisibleChanged: {
        //                     if (!visible && menuBarWrapper.activeMenu === fileMenu) {
        //                         menuBarWrapper.closeActiveMenu();
        //                     }
        //                 }

        //                 background: Rectangle {
        //                     anchors.fill: parent
        //                     color: _color.background_overlay
        //                     radius: 2
        //                 }

        //                 Column {
        //                     id: fileMenuColumn
        //                     anchors {
        //                         verticalCenter: parent.verticalCenter
        //                     }

        //                     width: parent.width
        //                     spacing: 2
        //                     NpContextMenuButton {
        //                         width: parent.width
        //                         text: "Open"
        //                         icon.source: "image://TiredImageProvider/folder.svg"
        //                         font: _fonts.label_accent
        //                         onClicked: {
        //                             fileMenu.close();
        //                             fileDialog.show();
        //                         }
        //                         topLeftRadius: 2
        //                         topRightRadius: 2
        //                     }
        //                     NpContextMenuButton {
        //                         width: parent.width
        //                         text: "Save"
        //                         icon.source: "image://TiredImageProvider/save.svg"
        //                         font: _fonts.label_accent
        //                         onClicked: {
        //                             fileMenu.close();
        //                             fileDialog.show();
        //                         }
        //                     }
        //                     NpContextMenuButton {
        //                         width: parent.width
        //                         text: "Exit"
        //                         icon.source: "image://TiredImageProvider/door-open.svg"
        //                         font: _fonts.label_accent
        //                         onClicked: {
        //                             fileMenu.close();
        //                             MainWindow.quitApplication();
        //                         }
        //                         bottomLeftRadius: 2
        //                         bottomRightRadius: 2
        //                     }
        //                 }
        //             }
        //         }

        //         NpMenuBarButton {
        //             id: helpMenuButton

        //             Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

        //             Layout.preferredHeight: 24

        //             text: "Help"
        //             font: _fonts.label_accent

        //             onClicked: {
        //                 if (menuBarWrapper.activeMenu === helpMenu) {
        //                     menuBarWrapper.closeActiveMenu();
        //                 } else {
        //                     menuBarWrapper.openMenu(helpMenu);
        //                 }
        //             }

        //             onHoveredChanged: {
        //                 if (hovered && menuBarWrapper.isMenuActive && menuBarWrapper.activeMenu !== helpMenu) {
        //                     menuBarWrapper.openMenu(helpMenu);
        //                 }
        //             }

        //             backgroundColor: "transparent"

        //             Popup {
        //                 id: helpMenu

        //                 y: parent.height

        //                 width: 256
        //                 height: helpMenuColumn.implicitHeight

        //                 padding: 0

        //                 popupType: Popup.Window

        //                 background: Rectangle {
        //                     anchors.fill: parent
        //                     color: _color.background_overlay
        //                     radius: 2
        //                 }

        //                 Column {
        //                     id: helpMenuColumn
        //                     width: parent.width
        //                     NpContextMenuButton {
        //                         width: parent.width
        //                         height: 32
        //                         text: "About..."
        //                         icon.source: "image://TiredImageProvider/circle-exclamation.svg"
        //                         font: _fonts.label_accent
        //                         onClicked: {
        //                             helpMenu.close();
        //                         }
        //                         topLeftRadius: 2
        //                         topRightRadius: 2
        //                         bottomLeftRadius: 2
        //                         bottomRightRadius: 2
        //                     }
        //                 }
        //             }
        //         }

        //         Item {
        //             Layout.fillWidth: true
        //         }
        //     }

        Rectangle {
            id: hatArea
            anchors {
                top: parent.top
                left: parent.left
                leftMargin: 256
                right: parent.right
                rightMargin: 256
            }
            height: parent.height * 0.8
            color: _color.mode_system

            border {
                width: 2
                color: _color.popup_header
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

                enabled: false
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

                enabled: false
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
        // }
    }
}
