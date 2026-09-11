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

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    color: _color.si_button_shadow

    // topLeftRadius: _units.radiusEight
    // topRightRadius: _units.radiusEight

    // MouseArea {
    //     id: topEdgeMoveArea
    //     anchors {
    //         left: parent.left
    //         right: parent.right
    //         top: parent.top
    //     }
    //     height: _units.scaled_4
    //     cursorShape: Qt.SizeVerCursor
    //     onPressed: {
    //         MainWindow.resizeWindow(Qt.TopEdge);
    //     }
    // }

    // MouseArea {
    //     id: windowMoveArea
    //     anchors {
    //         left: parent.left
    //         right: parent.right
    //         top: topEdgeMoveArea.bottom
    //         bottom: parent.bottom
    //     }
    //     onPressed: {
    //         MainWindow.moveWindow();
    //     }
    // }

    NpFileDialog {
        id: fileDialog
    }

    Item {
        id: topPanelMainComponentWrapper
        anchors.fill: parent

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
        //             leftMargin: _units.full
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
                topMargin: -border.width
                left: parent.left
                leftMargin: _units.scaled_256
                right: parent.right
                rightMargin: _units.scaled_256
            }

            height: parent.height * 0.7
            color: _color.si_mode_system

            border {
                width: _units.scaled_2
                color: _color.si_background_dark
            }

            states: [
                State {
                    name: "scene_mode"
                    when: Tired.controlMode == 0
                    PropertyChanges {
                        target: hatArea
                        color: _color.si_mode_scene
                    }
                },
                State {
                    name: "system_mode"
                    when: Tired.controlMode == 1
                    PropertyChanges {
                        target: hatArea
                        color: _color.si_mode_system
                    }
                }
            ]

            property var buttonsList: []

            function deselectOthers(exceptItem) {
                for (let i in hatArea.buttonsList) {
                    const btn = hatArea.buttonsList[i];
                    if (btn !== exceptItem) {
                        btn.checked = false;
                    }
                }
            }

            ModeSelectButton {
                id: sceneModeButton

                checked: true

                anchors {
                    right: systemModeButton.left
                    top: parent.top
                    bottom: parent.bottom
                }

                width: _units.scaled_96

                buttonLabel: "Scene"

                onClicked: {
                    sceneModeButton.checked = !sceneModeButton.checked;

                    if (sceneModeButton.checked) {
                        hatArea.deselectOthers(sceneModeButton);
                    }

                    // ControlModes::SCENE
                    Tired.controlMode = 0;
                }

                Component.onCompleted: {
                    hatArea.buttonsList.push(sceneModeButton);
                }
            }

            ModeSelectButton {
                id: systemModeButton
                anchors {
                    right: parent.right
                    rightMargin: _units.scaled_8
                    top: parent.top
                    bottom: parent.bottom
                }

                width: _units.scaled_96

                buttonLabel: "System"

                onClicked: {
                    systemModeButton.checked = !systemModeButton.checked;

                    if (systemModeButton.checked) {
                        hatArea.deselectOthers(systemModeButton);
                    }

                    // ControlModes::SYSTEM
                    Tired.controlMode = 1;
                }

                Component.onCompleted: {
                    hatArea.buttonsList.push(systemModeButton);
                }
            }
        }

        Rectangle {
            id: trackBallInfoArea
            anchors {
                bottom: parent.bottom
                topMargin: -border.width
                left: parent.left
                leftMargin: _units.scaled_256
                right: parent.right
                rightMargin: _units.scaled_256
            }

            height: parent.height * 0.25
            color: "transparent"

            border {
                width: _units.scaled_2
                color: _color.si_background_dark
            }

            Text {
                id: eyePosLabel
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    right: centerPosLabel.left
                    rightMargin: _units.scaled_8
                }

                verticalAlignment: Text.AlignVCenter

                width: implicitWidth

                font: _fonts.label
                color: _color.main_contrast
                text: {
                    const eye = Tired.manipulator.eye;
                    return `eye: ${eye.x.toFixed(3)}  ${eye.y.toFixed(3)}  ${eye.z.toFixed(3)}`;
                }
            }

            Text {
                id: centerPosLabel
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    right: parent.right
                    rightMargin: _units.scaled_8
                }

                verticalAlignment: Text.AlignVCenter

                width: implicitWidth

                font: _fonts.label
                color: _color.main_contrast
                text: {
                    const center = Tired.manipulator.center;
                    return `cnt: ${center.x.toFixed(3)}  ${center.y.toFixed(3)}  ${center.z.toFixed(3)}`;
                }
            }
        }
        // Item {
        //     id: mainWindowDecorationWrapper

        //     anchors {
        //         right: parent.right
        //         rightMargin: _units.full
        //         top: parent.top
        //         bottom: parent.bottom
        //     }

        //     width: _units.scaled_128

        //     NpButton {
        //         id: maximizeWindowButton

        //         anchors {
        //             right: foldWindowButton.left
        //             rightMargin: _units.scaled_4
        //             top: parent.top
        //         }

        //         width: _units.scaled_24
        //         height: _units.scaled_24

        //         icon.source: "image://TiredImageProvider/arrow-up-right-from-square.svg"
        //         onClicked: {}

        //         enabled: false
        //     }

        //     NpButton {
        //         id: foldWindowButton

        //         anchors {
        //             right: closeAppButton.left
        //             rightMargin: 4
        //             top: parent.top
        //         }

        //         width: _units.scaled_24
        //         height: _units.scaled_24

        //         icon.source: "image://TiredImageProvider/square-minus.svg"
        //         onClicked: {}

        //         enabled: false
        //     }

        //     NpButton {
        //         id: closeAppButton

        //         anchors {
        //             right: parent.right
        //             top: parent.top
        //         }

        //         width: _units.scaled_24
        //         height: _units.scaled_24

        //         icon.source: "image://TiredImageProvider/xmark-large.svg"
        //         onClicked: {
        //             MainWindow.quitApplication();
        //         }
        //     }
        // }
        // }
    }
}
