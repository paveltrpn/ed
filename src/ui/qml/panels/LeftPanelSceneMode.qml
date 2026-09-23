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

    property string activeBtn: ""

    Item {
        id: leftPanelMainComponentWrapper
        anchors {
            fill: parent
            leftMargin: leftPanelSceneComponent._units.scaled_2
            rightMargin: leftPanelSceneComponent._units.scaled_2
        }

        MouseArea {
            id: closePopupsMouseArea
            anchors.fill: parent
            onClicked: {
                leftPanelSceneComponent.activeBtn = "";
            }
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

            checked: leftPanelSceneComponent.activeBtn === "objectAddButton"

            onClicked: {
                if (objectAddButton.checked) {
                    leftPanelSceneComponent.activeBtn = "";
                    return;
                }

                leftPanelSceneComponent.activeBtn = "objectAddButton";
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

            checked: leftPanelSceneComponent.activeBtn === "meshAddButton"

            onClicked: {
                if (meshAddButton.checked) {
                    leftPanelSceneComponent.activeBtn = "";
                    return;
                }

                leftPanelSceneComponent.activeBtn = "meshAddButton";
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

            checked: leftPanelSceneComponent.activeBtn === "bezierAddButton"

            onClicked: {
                if (bezierAddButton.checked) {
                    leftPanelSceneComponent.activeBtn = "";
                    return;
                }

                leftPanelSceneComponent.activeBtn = "bezierAddButton";
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

            checked: leftPanelSceneComponent.activeBtn === "implicitAddButton"

            onClicked: {
                if (implicitAddButton.checked) {
                    leftPanelSceneComponent.activeBtn = "";
                    return;
                }

                leftPanelSceneComponent.activeBtn = "implicitAddButton";
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

            checked: leftPanelSceneComponent.activeBtn === "landscapeAddButton"

            onClicked: {
                if (landscapeAddButton.checked) {
                    leftPanelSceneComponent.activeBtn = "";
                    return;
                }

                leftPanelSceneComponent.activeBtn = "landscapeAddButton";
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

            checked: leftPanelSceneComponent.activeBtn === "polytopeAddButton"

            onClicked: {
                if (polytopeAddButton.checked) {
                    leftPanelSceneComponent.activeBtn = "";
                    return;
                }

                leftPanelSceneComponent.activeBtn = "polytopeAddButton";
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
            id: infoButton

            anchors {
                top: polytopeAddPopupItem.bottom
                topMargin: leftPanelSceneComponent._units.half
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_scene
            buttonLabel: "Info"

            checked: leftPanelSceneComponent.activeBtn === "infoButton"

            onClicked: {
                if (infoButton.checked) {
                    leftPanelSceneComponent.activeBtn = "";
                    return;
                }

                leftPanelSceneComponent.activeBtn = "infoButton";
            }
        }

        SceneInfoPanel {
            id: sceneinfoPanelItem
            anchors {
                top: infoButton.bottom
                topMargin: _units.eight
                left: parent.left
                right: parent.right
            }

            visible: infoButton.checked
        }

        SIButtonMain {
            id: settingsButton

            anchors {
                bottom: settingsPanelItem.top
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_scene
            buttonLabel: "Settings"

            checked: leftPanelSceneComponent.activeBtn === "settingsButton"

            onClicked: {
                if (settingsButton.checked) {
                    leftPanelSceneComponent.activeBtn = "";
                    return;
                }

                leftPanelSceneComponent.activeBtn = "settingsButton";
            }
        }

        SettingsPanel {
            id: settingsPanelItem
            anchors {
                bottom: parent.bottom
                bottomMargin: visible ? _units.half : 0
                left: parent.left
                right: parent.right
            }

            visible: settingsButton.checked
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
