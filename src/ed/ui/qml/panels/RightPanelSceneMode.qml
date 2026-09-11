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

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    color: _color.si_button_shadow

    Item {
        id: rightPanelScenemodeWrapper
        anchors {
            fill: parent
            leftMargin: rightPanelSceneComponent._units.scaled_2
            rightMargin: rightPanelSceneComponent._units.scaled_2
        }

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

                anchors {
                    top: parent.top

                    left: parent.left
                    right: parent.right
                }

                modeRelatedBgColor: _color.si_mode_scene
                height: _units.scaled_56
                buttonLabel: "ScaleX"
                text: " ... "
            }

            SIButtonIndicator {
                id: scaleYButton

                anchors {
                    top: scaleXButton.bottom
                    left: parent.left
                    right: parent.right
                }

                modeRelatedBgColor: _color.si_mode_scene
                height: _units.scaled_56
                buttonLabel: "ScaleY"
                text: " ... "
            }

            SIButtonIndicator {
                id: scaleZButton

                anchors {
                    top: scaleYButton.bottom
                    left: parent.left
                    right: parent.right
                }

                modeRelatedBgColor: _color.si_mode_scene
                height: _units.scaled_56
                buttonLabel: "ScaleZ"
                text: " ... "
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

                anchors {
                    top: parent.top

                    left: parent.left
                    right: parent.right
                }

                modeRelatedBgColor: _color.si_mode_scene
                height: _units.scaled_56
                buttonLabel: "RotX"
                text: " ... "
            }

            SIButtonIndicator {
                id: rotYButton

                anchors {
                    top: rotXButton.bottom
                    left: parent.left
                    right: parent.right
                }

                modeRelatedBgColor: _color.si_mode_scene
                height: _units.scaled_56
                buttonLabel: "RotY"
                text: " ... "
            }

            SIButtonIndicator {
                id: rotZButton

                anchors {
                    top: rotYButton.bottom
                    left: parent.left
                    right: parent.right
                }

                modeRelatedBgColor: _color.si_mode_scene
                height: _units.scaled_56
                buttonLabel: "RotZ"
                text: " ... "
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

                anchors {
                    top: parent.top

                    left: parent.left
                    right: parent.right
                }

                modeRelatedBgColor: _color.si_mode_scene
                height: _units.scaled_56
                buttonLabel: "TransX"
                text: " ... "
            }

            SIButtonIndicator {
                id: transYButton

                anchors {
                    top: transXButton.bottom
                    left: parent.left
                    right: parent.right
                }

                modeRelatedBgColor: _color.si_mode_scene
                height: _units.scaled_56
                buttonLabel: "TransY"
                text: " ... "
            }

            SIButtonIndicator {
                id: transZButton

                anchors {
                    top: transYButton.bottom
                    left: parent.left
                    right: parent.right
                }

                modeRelatedBgColor: _color.si_mode_scene
                height: _units.scaled_56
                buttonLabel: "TransZ"
                text: " ... "
            }
        }
    }
}
