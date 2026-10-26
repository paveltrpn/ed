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
    id: rightPanelSceneComponent

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    color: _color.si_button_shadow

    property string activeBtn: ""

    focus: true

    Item {
        id: rightPanelScenemodeWrapper
        anchors {
            fill: parent
            leftMargin: rightPanelSceneComponent._units.scaled_2
            rightMargin: rightPanelSceneComponent._units.scaled_2
        }

        property var selectedObjectHandle: null

        Connections {
            target: Tired.scenegraph.scene
            function onSelectedObjectChanged(object) {
                rightPanelScenemodeWrapper.selectedObjectHandle = object;

                if (object === null) {
                    scaleXButton.text = "0.000";
                    scaleYButton.text = "0.000";
                    scaleZButton.text = "0.000";
                    rotXButton.text = "0.000";
                    rotYButton.text = "0.000";
                    rotZButton.text = "0.000";
                    transXButton.text = "0.000";
                    transYButton.text = "0.000";
                    transZButton.text = "0.000";

                    return;
                }

                rightPanelScenemodeWrapper.syncTexts();
            }
        }

        Connections {
            target: rightPanelScenemodeWrapper.selectedObjectHandle

            function onPositionChanged() {
                rightPanelScenemodeWrapper.syncTexts();
            }

            function onOrientationChanged() {
                rightPanelScenemodeWrapper.syncTexts();
            }

            function onScaleChanged() {
                rightPanelScenemodeWrapper.syncTexts();
            }
        }

        function syncTexts() {
            const o = selectedObjectHandle;

            if (o === null)
                return;

            if (!scaleXButton.inputActiveFocus)
                scaleXButton.text = o.scale.x.toFixed(3);
            if (!scaleYButton.inputActiveFocus)
                scaleYButton.text = o.scale.y.toFixed(3);
            if (!scaleZButton.inputActiveFocus)
                scaleZButton.text = o.scale.z.toFixed(3);

            if (!rotXButton.inputActiveFocus)
                rotXButton.text = o.orientation.x.toFixed(3);
            if (!rotYButton.inputActiveFocus)
                rotYButton.text = o.orientation.y.toFixed(3);
            if (!rotZButton.inputActiveFocus)
                rotZButton.text = o.orientation.z.toFixed(3);

            if (!transXButton.inputActiveFocus)
                transXButton.text = o.position.x.toFixed(3);
            if (!transYButton.inputActiveFocus)
                transYButton.text = o.position.y.toFixed(3);
            if (!transZButton.inputActiveFocus)
                transZButton.text = o.position.z.toFixed(3);
        }

        Item {
            id: scaleButtonsWrapper

            anchors {
                top: parent.top
                topMargin: rightPanelSceneComponent._units.quarter
                left: parent.left
                right: parent.right
            }

            height: scaleXButton.height + scaleYButton.height + scaleZButton.height
            SIButtonIndicator {
                id: scaleXButton

                pressAreaAlignment: 1

                anchors {
                    top: parent.top

                    left: parent.left
                    right: parent.right
                }

                modeRelatedBgColor: _color.si_mode_scene

                buttonLabel: "ScaleX"

                text: "1.000"

                onTextValueChanged: function (value) {
                    if (rightPanelScenemodeWrapper.selectedObjectHandle === null) {
                        return;
                    }
                    let nowScale = rightPanelScenemodeWrapper.selectedObjectHandle.scale;
                    nowScale.x = Number.parseFloat(value);
                    rightPanelScenemodeWrapper.selectedObjectHandle.scale = nowScale;
                }
            }

            SIButtonIndicator {
                id: scaleYButton

                pressAreaAlignment: 1

                anchors {
                    top: scaleXButton.bottom
                    left: parent.left
                    right: parent.right
                }

                modeRelatedBgColor: _color.si_mode_scene
                buttonLabel: "ScaleY"

                text: "1.000"

                onTextValueChanged: function (value) {
                    if (rightPanelScenemodeWrapper.selectedObjectHandle === null) {
                        return;
                    }
                    let nowScale = rightPanelScenemodeWrapper.selectedObjectHandle.scale;
                    nowScale.y = Number.parseFloat(value);
                    rightPanelScenemodeWrapper.selectedObjectHandle.scale = nowScale;
                }
            }

            SIButtonIndicator {
                id: scaleZButton

                pressAreaAlignment: 1

                anchors {
                    top: scaleYButton.bottom
                    left: parent.left
                    right: parent.right
                }

                modeRelatedBgColor: _color.si_mode_scene
                buttonLabel: "ScaleZ"

                text: "1.000"

                onTextValueChanged: function (value) {
                    if (rightPanelScenemodeWrapper.selectedObjectHandle === null) {
                        return;
                    }
                    let nowScale = rightPanelScenemodeWrapper.selectedObjectHandle.scale;
                    nowScale.z = Number.parseFloat(value);
                    rightPanelScenemodeWrapper.selectedObjectHandle.scale = nowScale;
                }
            }
        }

        Item {
            id: rotButtonsWrapper

            anchors {
                top: scaleButtonsWrapper.bottom
                topMargin: rightPanelSceneComponent._units.half
                left: parent.left
                right: parent.right
            }

            height: rotXButton.height + rotYButton.height + rotZButton.height

            SIButtonIndicator {
                id: rotXButton

                pressAreaAlignment: 1

                anchors {
                    top: parent.top

                    left: parent.left
                    right: parent.right
                }

                modeRelatedBgColor: _color.si_mode_scene
                buttonLabel: "RotX"

                text: "0.000"

                onTextValueChanged: function (value) {
                    if (rightPanelScenemodeWrapper.selectedObjectHandle === null) {
                        return;
                    }
                    let nowRot = rightPanelScenemodeWrapper.selectedObjectHandle.orientation;
                    nowRot.x = Number.parseFloat(value);
                    rightPanelScenemodeWrapper.selectedObjectHandle.orientation = nowRot;
                }
            }

            SIButtonIndicator {
                id: rotYButton

                pressAreaAlignment: 1

                anchors {
                    top: rotXButton.bottom
                    left: parent.left
                    right: parent.right
                }

                modeRelatedBgColor: _color.si_mode_scene
                buttonLabel: "RotY"

                text: "0.000"

                onTextValueChanged: function (value) {
                    if (rightPanelScenemodeWrapper.selectedObjectHandle === null) {
                        return;
                    }
                    let nowRot = rightPanelScenemodeWrapper.selectedObjectHandle.orientation;
                    nowRot.y = Number.parseFloat(value);
                    rightPanelScenemodeWrapper.selectedObjectHandle.orientation = nowRot;
                }
            }

            SIButtonIndicator {
                id: rotZButton

                pressAreaAlignment: 1

                anchors {
                    top: rotYButton.bottom
                    left: parent.left
                    right: parent.right
                }

                modeRelatedBgColor: _color.si_mode_scene
                buttonLabel: "RotZ"

                text: "0.000"

                onTextValueChanged: function (value) {
                    if (rightPanelScenemodeWrapper.selectedObjectHandle === null) {
                        return;
                    }
                    let nowRot = rightPanelScenemodeWrapper.selectedObjectHandle.orientation;
                    nowRot.z = Number.parseFloat(value);
                    rightPanelScenemodeWrapper.selectedObjectHandle.orientation = nowRot;
                }
            }
        }

        Item {
            id: transButtonsWrapper

            anchors {
                top: rotButtonsWrapper.bottom
                topMargin: rightPanelSceneComponent._units.half
                left: parent.left
                right: parent.right
            }

            height: transXButton.height + transYButton.height + transZButton.height

            SIButtonIndicator {
                id: transXButton

                pressAreaAlignment: 1

                anchors {
                    top: parent.top

                    left: parent.left
                    right: parent.right
                }

                modeRelatedBgColor: _color.si_mode_scene
                buttonLabel: "TransX"

                text: "0.000"

                onTextValueChanged: function (value) {
                    if (rightPanelScenemodeWrapper.selectedObjectHandle === null) {
                        return;
                    }
                    let nowPos = rightPanelScenemodeWrapper.selectedObjectHandle.position;
                    nowPos.x = Number.parseFloat(value);
                    rightPanelScenemodeWrapper.selectedObjectHandle.position = nowPos;
                }
            }

            SIButtonIndicator {
                id: transYButton

                pressAreaAlignment: 1

                anchors {
                    top: transXButton.bottom
                    left: parent.left
                    right: parent.right
                }

                modeRelatedBgColor: _color.si_mode_scene
                buttonLabel: "TransY"

                text: "0.000"

                onTextValueChanged: function (value) {
                    if (rightPanelScenemodeWrapper.selectedObjectHandle === null) {
                        return;
                    }
                    let nowPos = rightPanelScenemodeWrapper.selectedObjectHandle.position;
                    nowPos.y = Number.parseFloat(value);
                    rightPanelScenemodeWrapper.selectedObjectHandle.position = nowPos;
                }
            }

            SIButtonIndicator {
                id: transZButton

                pressAreaAlignment: 1

                anchors {
                    top: transYButton.bottom
                    left: parent.left
                    right: parent.right
                }

                modeRelatedBgColor: _color.si_mode_scene
                buttonLabel: "TransZ"

                text: "0.000"

                onTextValueChanged: function (value) {
                    if (rightPanelScenemodeWrapper.selectedObjectHandle === null) {
                        return;
                    }
                    let nowPos = rightPanelScenemodeWrapper.selectedObjectHandle.position;
                    nowPos.z = Number.parseFloat(value);
                    rightPanelScenemodeWrapper.selectedObjectHandle.position = nowPos;
                }
            }
        }

        SIButtonMain {
            id: deselectObjectsButton

            anchors {
                bottom: sceneButton.top
                bottomMargin: _units.eight
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_scene
            buttonLabel: "Deselect"

            enabled: Tired.scenegraph.scene.isAnyObjectSelected

            onClicked: {
                Tired.scenegraph.scene.selectedObjectUid = ""
                mainWindow.panelsActiveBtn = "";
            }
        }

        SIButtonMain {
            id: sceneButton

            anchors {
                bottom: sceneinfoPanelItem.top
                bottomMargin: _units.quarter
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: _color.si_mode_scene
            buttonLabel: "Scene"

            checked: mainWindow.panelsActiveBtn === "infoButton"

            onClicked: {
                if (sceneButton.checked) {
                    mainWindow.panelsActiveBtn = "";
                    return;
                }

                mainWindow.panelsActiveBtn = "infoButton";
            }
        }

        SceneInfoPanel {
            id: sceneinfoPanelItem

            anchors {
                bottom: rightPanelScenemodeWrapper.bottom
                bottomMargin: visible ? _units.quarter : 0
                left: parent.left
                right: parent.right
            }

            visible: sceneButton.checked
        }
    }
}
