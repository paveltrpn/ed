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
    id: leftPanelSceneComponent

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    color: _color.si_button_shadow

    Item {
        id: leftPanelMainComponentWrapper
        anchors {
            fill: parent
            leftMargin: leftPanelSceneComponent._units.scaled_2
            rightMargin: leftPanelSceneComponent._units.scaled_2
        }

        SIButtonMain {
            id: objectAddButton

            anchors {
                top: parent.top
                topMargin: leftPanelSceneComponent._units.half
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_scene
            buttonLabel: "Object"

            checked: mainWindow.panelsActiveBtn === "objectAddButton"

            onClicked: {
                if (objectAddButton.checked) {
                    mainWindow.panelsActiveBtn = "";
                    return;
                }

                mainWindow.panelsActiveBtn = "objectAddButton";
            }
        }

        ObjectAddPopup {
            id: objectAddPopupItem

            anchors {
                top: objectAddButton.bottom
                topMargin: visible ? _units.eight : 0
                left: parent.left
                right: parent.right
            }

            visible: objectAddButton.checked
        }

        SIButtonMain {
            id: meshAddButton

            anchors {
                top: objectAddPopupItem.bottom
                topMargin: leftPanelSceneComponent._units.half
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_scene
            buttonLabel: "Mesh"

            checked: mainWindow.panelsActiveBtn === "meshAddButton"

            onClicked: {
                if (meshAddButton.checked) {
                    mainWindow.panelsActiveBtn = "";
                    return;
                }

                mainWindow.panelsActiveBtn = "meshAddButton";
            }
        }

        MeshAddPopup {
            id: meshAddPopupItem

            anchors {
                top: meshAddButton.bottom
                topMargin: visible ? _units.eight : 0
                left: parent.left
                right: parent.right
            }

            visible: meshAddButton.checked
        }

        SIButtonMain {
            id: bezierAddButton

            anchors {
                top: meshAddPopupItem.bottom
                topMargin: leftPanelSceneComponent._units.half
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_scene
            buttonLabel: "Bezier"

            checked: mainWindow.panelsActiveBtn === "bezierAddButton"

            onClicked: {
                if (bezierAddButton.checked) {
                    mainWindow.panelsActiveBtn = "";
                    return;
                }

                mainWindow.panelsActiveBtn = "bezierAddButton";
            }
        }

        BezierAddPopup {
            id: bezierAddPopupItem

            anchors {
                top: bezierAddButton.bottom
                topMargin: visible ? _units.eight : 0
                left: parent.left
                right: parent.right
            }

            visible: bezierAddButton.checked
        }

        SIButtonMain {
            id: implicitAddButton

            anchors {
                top: bezierAddPopupItem.bottom
                topMargin: leftPanelSceneComponent._units.half
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_scene
            buttonLabel: "Implicit"

            checked: mainWindow.panelsActiveBtn === "implicitAddButton"

            onClicked: {
                if (implicitAddButton.checked) {
                    mainWindow.panelsActiveBtn = "";
                    return;
                }

                mainWindow.panelsActiveBtn = "implicitAddButton";
            }
        }

        ImplicitAddPopup {
            id: implicitAddPopupItem

            anchors {
                top: implicitAddButton.bottom
                topMargin: visible ? _units.eight : 0
                left: parent.left
                right: parent.right
            }

            visible: implicitAddButton.checked
        }

        SIButtonMain {
            id: landscapeAddButton

            anchors {
                top: implicitAddPopupItem.bottom
                topMargin: leftPanelSceneComponent._units.half
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_scene
            buttonLabel: "Landscape"

            checked: mainWindow.panelsActiveBtn === "landscapeAddButton"

            onClicked: {
                if (landscapeAddButton.checked) {
                    mainWindow.panelsActiveBtn = "";
                    return;
                }

                mainWindow.panelsActiveBtn = "landscapeAddButton";
            }
        }

        LandscapeAddPopup {
            id: landscapeAddPopupItem

            anchors {
                top: landscapeAddButton.bottom
                topMargin: visible ? _units.eight : 0
                left: parent.left
                right: parent.right
            }

            visible: landscapeAddButton.checked
        }

        SIButtonMain {
            id: polytopeAddButton

            anchors {
                top: landscapeAddPopupItem.bottom
                topMargin: leftPanelSceneComponent._units.half
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_scene
            buttonLabel: "Polytope"

            checked: mainWindow.panelsActiveBtn === "polytopeAddButton"

            onClicked: {
                if (polytopeAddButton.checked) {
                    mainWindow.panelsActiveBtn = "";
                    return;
                }

                mainWindow.panelsActiveBtn = "polytopeAddButton";
            }
        }

        PolytopeAddPopup {
            id: polytopeAddPopupItem

            anchors {
                top: polytopeAddButton.bottom
                topMargin: visible ? _units.eight : 0
                left: parent.left
                right: parent.right
            }

            visible: polytopeAddButton.checked
        }

        SIButtonMain {
            id: loadSceneButton

            anchors {
                bottom: saveSceneButton.top
                bottomMargin: _units.quarter
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_system
            buttonLabel: "Load"

            enabled: !saveSceneDialogItem.visible

            onClicked: {
                loadSceneDialogItem.visible = true;
            }
        }

        SIEmbeddedWidget {
            id: loadSceneDialogItem

            visible: false

            width: 640
            height: 480

            title: "Load scene"

            onAboutToClose: {
                loadSceneDialogItem.visible = false
            }

            onVisibleChanged: {
                if (loadSceneDialogItem.visible) {
                    const centerX = mainWindow.width / 2;
                    const centerY = mainWindow.height / 2;

                    loadSceneDialogItem.x = centerX - loadSceneDialogItem.width / 2;
                    loadSceneDialogItem.y = centerY - loadSceneDialogItem.height / 2;
                }
            }

            contentSource: "../components/FilesystemComponent.qml"
        }

        SIButtonMain {
            id: saveSceneButton

            anchors {
                bottom: parent.bottom
                bottomMargin: _units.quarter
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_system
            buttonLabel: "Save"

            enabled: !loadSceneDialogItem.visible

            onClicked: {
                saveSceneDialogItem.visible = true;
            }
        }

        SIEmbeddedWidget {
            id: saveSceneDialogItem

            visible: false

            width: 640
            height: 480

            title: "Save scene"

            onAboutToClose: {
                saveSceneDialogItem.visible = false
            }

            onVisibleChanged: {
                if (saveSceneDialogItem.visible) {
                    const centerX = mainWindow.width / 2;
                    const centerY = mainWindow.height / 2;

                    saveSceneDialogItem.x = centerX - saveSceneDialogItem.width / 2;
                    saveSceneDialogItem.y = centerY - saveSceneDialogItem.height / 2;
                }
            }

            contentSource: "../components/FilesystemComponent.qml"
        }

        // SettingsComponent {
        //     id: settingsWidget
        //     anchors {
        //         bottom: settingsButton.top
        //         bottomMargin: _units.half
        //         left: parent.left
        //         right: parent.right
        //     }

        //     visible: settingsButton.checked
        // }
    }
}
