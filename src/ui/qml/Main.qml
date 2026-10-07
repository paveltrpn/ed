// qmllint disable unqualified
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import Qt5Compat.GraphicalEffects

import Tire 1.0

import "panels"

Item {
    id: mainWindow

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    property string panelsActiveBtn: ""

    MouseArea {
        id: mainWindowFocusEater
        anchors.fill: parent
        onPressed: {
            parent.panelsActiveBtn = "";
            parent.forceActiveFocus();
        }
    }

    TopPanel {
        id: topPanel

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }

        height: appStateSettings.topPanelHeight

        property real _pressHeight: 0
        property real _pressMouseY: 0

        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            color: topPanelResizeHandle.containsMouse || topPanelResizeHandle.pressed ? _color.si_scrollbar_light : "transparent"
            height: topPanelResizeHandle.height
        }

        MouseArea {
            id: topPanelResizeHandle
            anchors {
                left: parent.left
                right: parent.right
                bottom: topPanelResizeHandle.pressed ? undefined : parent.bottom
            }

            height: _units.scaled_8

            cursorShape: Qt.SizeVerCursor

            onPressed: mouse => {
                topPanel._pressHeight = topPanel.height;
                topPanel._pressMouseY = mouse.y;
            }

            onPositionChanged: mouse => {
                if (!pressed)
                    return;

                const dy = mouse.y - topPanel._pressMouseY;
                const newHeight = topPanel._pressHeight + dy;

                appStateSettings.topPanelHeight = newHeight;
            }
        }
    }

    // ======================================================================================
    // ==================== Render view =====================================================
    // ======================================================================================
    Item {
        id: renderViewWrapper

        anchors {
            top: topPanel.bottom
            bottom: bottomPanel.top
            left: leftPanel.right
            right: rightPanel.left
        }

        Rectangle {
            id: viewInfoPanel
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }

            height: _units.scaled_24
            color: _color.si_additional_grey_faded

            border {
                width: _units.scaled_1
                color: _color.si_background_dark
            }

            clip: true

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
                color: _color.additional_contrast_60
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
                color: _color.additional_contrast_60
                text: {
                    const center = Tired.manipulator.center;
                    return `cnt: ${center.x.toFixed(3)}  ${center.y.toFixed(3)}  ${center.z.toFixed(3)}`;
                }
            }
        }

        Render {
            id: render
            anchors {
                top: topPanelResizeHandle.pressed ? undefined : viewInfoPanel.bottom
                bottom: bottomPanelResizeHandle.pressed ? undefined : parent.bottom
                left: leftPanelResizeHandle.pressed ? undefined : parent.left
                right: rightPanelResizeHandle.pressed ? undefined : parent.right
            }
        }
    }
    // ======================================================================================
    // ======================================================================================
    // ======================================================================================

    BottomPanel {
        id: bottomPanel

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        height: appStateSettings.bottomPanelHeight

        property real _pressHeight: 0
        property real _pressMouseY: 0

        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
            color: bottomPanelResizeHandle.containsMouse || bottomPanelResizeHandle.pressed ? _color.si_scrollbar_light : "transparent"
            height: bottomPanelResizeHandle.height
        }

        MouseArea {
            id: bottomPanelResizeHandle
            anchors {
                left: parent.left
                right: parent.right
                top: bottomPanelResizeHandle.pressed ? undefined : parent.top
            }

            height: _units.scaled_8

            cursorShape: Qt.SizeVerCursor

            onPressed: mouse => {
                bottomPanel._pressHeight = bottomPanel.height;
                bottomPanel._pressMouseY = mouse.y;
            }

            onPositionChanged: mouse => {
                if (!pressed)
                    return;

                const dy = mouse.y - bottomPanel._pressMouseY;
                const newHeight = bottomPanel._pressHeight - dy;

                appStateSettings.bottomPanelHeight = newHeight;
            }
        }
    }

    LeftPanel {
        id: leftPanel

        anchors {
            top: topPanel.bottom
            bottom: bottomPanel.top
            left: parent.left
        }

        width: appStateSettings.leftPanelWidth

        property real _pressWidth: 0
        property real _pressMouseX: 0

        Rectangle {
            anchors {
                top: parent.top
                bottom: parent.bottom
                right: parent.right
            }
            color: leftPanelResizeHandle.containsMouse || leftPanelResizeHandle.pressed ? _color.si_scrollbar_light : "transparent"
            width: leftPanelResizeHandle.width
        }

        MouseArea {
            id: leftPanelResizeHandle
            anchors {
                top: parent.top
                bottom: parent.bottom
                right: leftPanelResizeHandle.pressed ? undefined : parent.right
            }

            width: _units.scaled_8

            cursorShape: Qt.SizeHorCursor

            onPressed: mouse => {
                leftPanel._pressWidth = leftPanel.width;
                leftPanel._pressMouseX = mouse.x;
            }

            onPositionChanged: mouse => {
                if (!pressed)
                    return;

                const dx = mouse.x - leftPanel._pressMouseX;
                const newWidth = leftPanel._pressWidth + dx;

                appStateSettings.leftPanelWidth = newWidth;
            }
        }
    }

    RightPanel {
        id: rightPanel

        anchors {
            top: topPanel.bottom
            bottom: bottomPanel.top
            right: parent.right
        }

        width: appStateSettings.rightPanelWidth

        property real _pressWidth: 0
        property real _pressMouseX: 0

        Rectangle {
            anchors {
                top: parent.top
                bottom: parent.bottom
                left: parent.left
            }
            color: rightPanelResizeHandle.containsMouse || rightPanelResizeHandle.pressed ? _color.si_scrollbar_light : "transparent"
            width: rightPanelResizeHandle.width
        }

        MouseArea {
            id: rightPanelResizeHandle
            anchors {
                top: parent.top
                bottom: parent.bottom
                left: rightPanelResizeHandle.pressed ? undefined : parent.left
            }

            width: _units.scaled_8

            cursorShape: Qt.SizeHorCursor

            onPressed: mouse => {
                rightPanel._pressWidth = rightPanel.width;
                rightPanel._pressMouseX = mouse.x;
            }

            onPositionChanged: mouse => {
                if (!pressed)
                    return;

                const dx = mouse.x - rightPanel._pressMouseX;
                const newWidth = rightPanel._pressWidth - dx;

                appStateSettings.rightPanelWidth = newWidth;
            }
        }
    }

    // Rectangle {
    //     anchors {
    //         // right: parent.right
    //         // bottom: parent.bottom

    //         centerIn: parent
    //     }

    //     radius: 8
    //     width: clickMessage.implicitWidth + 32
    //     height: 64

    //     color: "#4cff0000"

    //     Text {
    //         id: clickMessage
    //         anchors {
    //             fill: parent
    //         }

    //         text: "click me"
    //         color: "white"
    //         font: Qt.font({
    //             "pixelSize": 16,
    //             "weight": Font.ExtraBold,
    //             "family": "Monospace"
    //         })

    //         horizontalAlignment: Text.AlignHCenter
    //         verticalAlignment: Text.AlignVCenter
    //     }

    //     MouseArea {
    //         anchors.fill: parent
    //         onClicked: {
    //             mainWindow.greenExpanded = !mainWindow.greenExpanded;
    //             console.log(" i am clickable!!! ");
    //         }
    //     }
    // }
}
