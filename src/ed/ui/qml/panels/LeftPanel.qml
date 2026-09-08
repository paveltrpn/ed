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

            property var buttonsList: []

            NpButton {
                id: addSceneObjBtn
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                icon.source: "image://TiredImageProvider/shapes-svgrepo-com.svg"

                onClicked: {
                    addSceneObjBtn.checked = !addSceneObjBtn.checked;
                    if (addSceneObjBtn.checked) {
                        mainButtonsLayout.deselectOthers(addSceneObjBtn);
                    }
                }

                Component.onCompleted: {
                    mainButtonsLayout.buttonsList.push(addSceneObjBtn);
                }
            }

            NpButton {
                id: editSceneObjBtn
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                icon.source: "image://TiredImageProvider/wand-magic-svgrepo-com.svg"

                onClicked: {
                    editSceneObjBtn.checked = !editSceneObjBtn.checked;
                    if (editSceneObjBtn.checked) {
                        mainButtonsLayout.deselectOthers(editSceneObjBtn);
                    }
                }

                Component.onCompleted: {
                    mainButtonsLayout.buttonsList.push(editSceneObjBtn);
                }
            }

            NpButton {
                id: showSceneInfoBtn
                Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                icon.source: "image://TiredImageProvider/info.svg"

                onClicked: {
                    showSceneInfoBtn.checked = !showSceneInfoBtn.checked;

                    if (showSceneInfoBtn.checked) {
                        mainButtonsLayout.deselectOthers(showSceneInfoBtn);
                    }
                }

                Component.onCompleted: {
                    mainButtonsLayout.buttonsList.push(showSceneInfoBtn);
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
                        mainButtonsLayout.deselectOthers(settingsBtn);
                    }
                }

                Component.onCompleted: {
                    mainButtonsLayout.buttonsList.push(settingsBtn);
                }
            }

            Item {
                id: spacerBottom
                Layout.preferredHeight: _gaps.half
            }

            function deselectOthers(exceptItem) {
                for (let i in mainButtonsLayout.buttonsList) {
                    const btn = mainButtonsLayout.buttonsList[i];
                    if (btn !== exceptItem) {
                        btn.checked = false;
                    }
                }
            }
        }
    }

    SIButtonMain {
        id: addObjectButton

        anchors {
            top: mainLeftPanel.top
            left: mainLeftPanel.right
            leftMargin: leftPanelMainComponent._gaps.half
            right: parent.right
            rightMargin: leftPanelMainComponent._gaps.half
        }

        height: 56

        buttonLabel: "Add object"

        onClicked: {
            addObjectButton.checked = !addObjectButton.checked;
        }
    }

    SIButtonMain {
        id: editObjectButton

        anchors {
            top: addObjectButton.bottom
            topMargin: leftPanelMainComponent._gaps.half
            left: mainLeftPanel.right
            leftMargin: leftPanelMainComponent._gaps.half
            right: parent.right
            rightMargin: leftPanelMainComponent._gaps.half
        }

        height: 56

        buttonLabel: "Edit object"

        onClicked: {
            editObjectButton.checked = !editObjectButton.checked;
        }
    }

    SIButtonMain {
        id: infoButton

        anchors {
            top: editObjectButton.bottom
            topMargin: leftPanelMainComponent._gaps.half
            left: mainLeftPanel.right
            leftMargin: leftPanelMainComponent._gaps.half
            right: parent.right
            rightMargin: leftPanelMainComponent._gaps.half
        }

        height: 56

        buttonLabel: "Info"

        onClicked: {
            infoButton.checked = !infoButton.checked;
        }
    }

    SIButtonMain {
        id: settingsButton

        anchors {
            bottom: mainLeftPanel.bottom
            left: mainLeftPanel.right
            leftMargin: leftPanelMainComponent._gaps.half
            right: parent.right
            rightMargin: leftPanelMainComponent._gaps.half
        }

        height: 56

        buttonLabel: "Settings"

        onClicked: {
            settingsButton.checked = !settingsButton.checked;
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

        visible: showSceneInfoBtn.checked
    }

    AddPrimitivesPopup {
        id: addPrimitivesPopupItem

        anchors {
            top: mainLeftPanel.top
            left: mainLeftPanel.right
            leftMargin: leftPanelMainComponent._gaps.half
            right: parent.right
            rightMargin: leftPanelMainComponent._gaps.half
        }

        visible: addSceneObjBtn.checked
    }
}
