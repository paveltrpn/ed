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

    Item {
        id: rightPanelScenemodeWrapper
        anchors {
            fill: parent
            leftMargin: rightPanelSceneComponent._units.scaled_2
            rightMargin: rightPanelSceneComponent._units.scaled_2
        }

        property vector3d position: Qt.vector3d(0, 0, 0)
        property vector3d orientation: Qt.vector3d(0, 0, 0)
        property vector3d scale: Qt.vector3d(0, 0, 0)

        property var selectedObjectHandle: null

        Connections {
            target: Tired.scenegraph.scene
            function onSelectedObjectChanged(object) {
                rightPanelScenemodeWrapper.selectedObjectHandle = object

                if (object === null) {
                    rightPanelScenemodeWrapper.position = Qt.vector3d(0, 0, 0)
                    rightPanelScenemodeWrapper.orientation = Qt.vector3d(0, 0, 0)
                    rightPanelScenemodeWrapper.scale = Qt.vector3d(0, 0, 0)

                    return;
                }

                rightPanelScenemodeWrapper.position = object.position
                rightPanelScenemodeWrapper.orientation = object.orientation
                rightPanelScenemodeWrapper.scale = object.scale
            }
        }

        Item {
            id: scaleButtonsWrapper

            anchors {
                top: parent.top
                topMargin: rightPanelSceneComponent._units.half
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
                text: rightPanelScenemodeWrapper.scale.x.toFixed(3)
                onTextValueChanged: function (value) {
                    if (rightPanelScenemodeWrapper.selectedObjectHandle === null) {
                        return
                    }
                    let nowScale = rightPanelScenemodeWrapper.selectedObjectHandle.scale
                    nowScale.x = Number.parseFloat(value)
                    rightPanelScenemodeWrapper.selectedObjectHandle.scale = nowScale
                    Tired.scenegraph.bounding.setOnObject(rightPanelScenemodeWrapper.selectedObjectHandle)
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
                text: rightPanelScenemodeWrapper.scale.y.toFixed(3)
                onTextValueChanged: function (value) {
                    if (rightPanelScenemodeWrapper.selectedObjectHandle === null) {
                        return
                    }
                    let nowScale = rightPanelScenemodeWrapper.selectedObjectHandle.scale
                    nowScale.y = Number.parseFloat(value)
                    rightPanelScenemodeWrapper.selectedObjectHandle.scale = nowScale
                    Tired.scenegraph.bounding.setOnObject(rightPanelScenemodeWrapper.selectedObjectHandle)
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
                text: rightPanelScenemodeWrapper.scale.z.toFixed(3)
                onTextValueChanged: function (value) {
                    if (rightPanelScenemodeWrapper.selectedObjectHandle === null) {
                        return
                    }
                    let nowScale = rightPanelScenemodeWrapper.selectedObjectHandle.scale
                    nowScale.z = Number.parseFloat(value)
                    rightPanelScenemodeWrapper.selectedObjectHandle.scale = nowScale
                    Tired.scenegraph.bounding.setOnObject(rightPanelScenemodeWrapper.selectedObjectHandle)
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
                text: rightPanelScenemodeWrapper.orientation.x.toFixed(3)
                onTextValueChanged: function (value) {
                    if (rightPanelScenemodeWrapper.selectedObjectHandle === null) {
                        return
                    }
                    let nowRot = rightPanelScenemodeWrapper.selectedObjectHandle.orientation
                    nowRot.x = Number.parseFloat(value)
                    rightPanelScenemodeWrapper.selectedObjectHandle.orientation = nowRot
                    Tired.scenegraph.bounding.setOnObject(rightPanelScenemodeWrapper.selectedObjectHandle)
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
                text: rightPanelScenemodeWrapper.orientation.y.toFixed(3)
                onTextValueChanged: function (value) {
                    if (rightPanelScenemodeWrapper.selectedObjectHandle === null) {
                        return
                    }
                    let nowRot = rightPanelScenemodeWrapper.selectedObjectHandle.orientation
                    nowRot.y = Number.parseFloat(value)
                    rightPanelScenemodeWrapper.selectedObjectHandle.orientation = nowRot
                    Tired.scenegraph.bounding.setOnObject(rightPanelScenemodeWrapper.selectedObjectHandle)
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
                text: rightPanelScenemodeWrapper.orientation.z.toFixed(3)

                onTextValueChanged: function (value) {
                    if (rightPanelScenemodeWrapper.selectedObjectHandle === null) {
                        return
                    }
                    let nowRot = rightPanelScenemodeWrapper.selectedObjectHandle.orientation
                    nowRot.z = Number.parseFloat(value)
                    rightPanelScenemodeWrapper.selectedObjectHandle.orientation = nowRot
                    Tired.scenegraph.bounding.setOnObject(rightPanelScenemodeWrapper.selectedObjectHandle)
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
                text: rightPanelScenemodeWrapper.position.x.toFixed(3)

                onTextValueChanged: function (value) {
                    if (rightPanelScenemodeWrapper.selectedObjectHandle === null) {
                        return
                    }
                    let nowPos = rightPanelScenemodeWrapper.selectedObjectHandle.position
                    nowPos.x = Number.parseFloat(value)
                    rightPanelScenemodeWrapper.selectedObjectHandle.position = nowPos
                    Tired.scenegraph.bounding.setOnObject(rightPanelScenemodeWrapper.selectedObjectHandle)
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
                text: rightPanelScenemodeWrapper.position.y.toFixed(3)
                onTextValueChanged: function (value) {
                    if (rightPanelScenemodeWrapper.selectedObjectHandle === null) {
                        return
                    }
                    let nowPos = rightPanelScenemodeWrapper.selectedObjectHandle.position
                    nowPos.y = Number.parseFloat(value)
                    rightPanelScenemodeWrapper.selectedObjectHandle.position = nowPos
                    Tired.scenegraph.bounding.setOnObject(rightPanelScenemodeWrapper.selectedObjectHandle)
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
                text: rightPanelScenemodeWrapper.position.z.toFixed(3)
                onTextValueChanged: function (value) {
                    if (rightPanelScenemodeWrapper.selectedObjectHandle === null) {
                        return
                    }
                    let nowPos = rightPanelScenemodeWrapper.selectedObjectHandle.position
                    nowPos.z = Number.parseFloat(value)
                    rightPanelScenemodeWrapper.selectedObjectHandle.position = nowPos
                    Tired.scenegraph.bounding.setOnObject(rightPanelScenemodeWrapper.selectedObjectHandle)
                }
            }
        }
    }
}
