// qmllint disable unqualified
// qmllint disable Quick.property-changes-parsed

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Tire 1.0

import "../components"
import "../components/filedialog"

Rectangle {
    id: bottomPanelBgComponent

    color: _color.si_additional_grey_faded

    Rectangle {
        id: sceneControlButtonsBottomWrapper

        anchors {
            top: parent.top
            topMargin: _units.scaled_4
            right: parent.right
            rightMargin: _units.scaled_4
            bottom: parent.bottom
            bottomMargin: _units.scaled_4
        }

        width: 256

        color: "transparent"

        border {
            width: _units.scaled_2
            color: _color.si_background_dark
        }

        ColumnLayout {
            id: controlButtonsPanelColumn

            spacing: _units.scaled_2

            anchors {
                right: parent.right
                rightMargin: _units.scaled_4
                left: parent.left
                leftMargin: _units.scaled_4
                top: parent.top
                topMargin: _units.scaled_4
                bottom: parent.bottom
                bottomMargin: _units.scaled_4
            }

            RowLayout {
                id: topRow

                spacing: _units.scaled_2

                Layout.fillWidth: true
                Layout.preferredHeight: parent.height / 2

                SISceneControlButton {
                    id: wireframeModeButton

                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height

                    buttonLabel: "WRF"

                    checked: Tired.scenegraph.scene.renderMode == 0

                    onClicked: {
                        Tired.scenegraph.scene.renderMode = 0
                    }
                }

                SISceneControlButton {
                    id: solidModeButton

                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height

                    buttonLabel: "SLD"

                    checked: Tired.scenegraph.scene.renderMode == 1

                    onClicked: {
                        Tired.scenegraph.scene.renderMode = 1
                    }
                }

                SISceneControlButton {
                    id: solidwireframeModeButton

                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height

                    buttonLabel: "S+W"

                    checked: Tired.scenegraph.scene.renderMode == 2

                    onClicked: {
                        Tired.scenegraph.scene.renderMode = 2
                    }
                }

                SISceneControlButton {
                    id: dummy1_ModeButton

                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height

                    buttonLabel: "---"
                }

                SISceneControlButton {
                    id: dummy2_ModeButton

                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height

                    buttonLabel: "---"
                }
            }

            RowLayout {
                id: bottomRow

                spacing: _units.scaled_2

                Layout.fillWidth: true
                Layout.preferredHeight: parent.height / 2

                SISceneControlButton {
                    id: gimoLocalModeButton

                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height

                    buttonLabel: "LCL"

                    onClicked: {
                        gimoLocalModeButton.checked = !gimoLocalModeButton.checked

                        if (gimoLocalModeButton.checked) {
                            gimoGlobalModeButton.checked = false
                        }
                    }
                }

                SISceneControlButton {
                    id: gimoGlobalModeButton

                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height

                    buttonLabel: "GBL"

                    onClicked: {
                        gimoGlobalModeButton.checked = !gimoGlobalModeButton.checked

                        if (gimoGlobalModeButton.checked) {
                            gimoLocalModeButton.checked = false
                        }
                    }
                }

                SISceneControlButton {
                    id: dummy13_ModeButton

                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height

                    buttonLabel: "---"
                }

                SISceneControlButton {
                    id: dummy14_ModeButton

                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height

                    buttonLabel: "---"
                }

                SISceneControlButton {
                    id: dummy15_ModeButton

                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height

                    buttonLabel: "---"
                }
            }
        }
    }
}
