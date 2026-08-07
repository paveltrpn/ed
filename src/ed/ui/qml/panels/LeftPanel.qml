// qmllint disable unqualified
// qmllint disable Quick.property-changes-parsed

import QtQuick
import QtQuick.Layouts

import Tire 1.0

import "../components"
import "../sceneinfo"
import "../add_forms"
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

    Rectangle {
        id: mainLeftPanel

        anchors {
            left: parent.left
            top: parent.top
            topMargin: _gaps.half
            bottom: parent.bottom
            bottomMargin: _gaps.half
        }

        width: 40

        color: leftPanelMainComponent._color.background_overlay_60

        topRightRadius: _radius.half
        bottomRightRadius: _radius.half
        topLeftRadius: 0
        bottomLeftRadius: 0

        ColumnLayout {
            id: mainButtonsLayout

            spacing: _gaps.half

            anchors {
                fill: parent
            }

            Item {
                id: spacerTop
                Layout.preferredHeight: _gaps.half
            }

            NpButton {
                id: addBoxBtn
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                icon.source: "image://TiredImageProvider/square-medical.svg"
                onClicked: {
                    addBoxBtn.checked = !addBoxBtn.checked;
                    if (addBoxBtn.checked) {
                        showInfoBtn.checked = false;
                        settingsBtn.checked = false;
                    }
                }
            }

            Item {
                id: buttonsSpacer
                Layout.fillHeight: true
            }

            NpButton {
                id: settingsBtn
                Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                icon.source: "image://TiredImageProvider/gear.svg"
                onClicked: {
                    settingsBtn.checked = !settingsBtn.checked;

                    if (settingsBtn.checked) {
                        addBoxBtn.checked = false;
                        showInfoBtn.checked = false;
                    }
                }
            }

            NpButton {
                id: showInfoBtn
                Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                icon.source: "image://TiredImageProvider/info.svg"
                onClicked: {
                    showInfoBtn.checked = !showInfoBtn.checked;

                    if (showInfoBtn.checked) {
                        addBoxBtn.checked = false;
                        settingsBtn.checked = false;
                    }
                }
            }

            Item {
                id: spacerBottom
                Layout.preferredHeight: _gaps.half
            }
        }
    }

    SettingsComponent {
        id: settingsWidget
        anchors {
            top: mainLeftPanel.top
            left: mainLeftPanel.right
            leftMargin: leftPanelMainComponent._gaps.half
            right: parent.right
            rightMargin: leftPanelMainComponent._gaps.half
        }

        visible: settingsBtn.checked
    }

    SceneInfoComponent {
        id: sceneInfoWidget
        anchors {
            top: mainLeftPanel.top
            left: mainLeftPanel.right
            leftMargin: leftPanelMainComponent._gaps.half
            right: parent.right
            rightMargin: leftPanelMainComponent._gaps.half
        }

        visible: showInfoBtn.checked
    }

    AddBox {
        id: addBoxWidget
        anchors {
            top: mainLeftPanel.top
            left: mainLeftPanel.right
            leftMargin: leftPanelMainComponent._gaps.half
            right: parent.right
            rightMargin: leftPanelMainComponent._gaps.half
        }

        visible: addBoxBtn.checked
    }
}
