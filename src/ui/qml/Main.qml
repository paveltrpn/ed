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

    // ======================================================================================
    // ==================== Render ==========================================================
    // ======================================================================================
    Render {
        id: render

        // width: parent.width
        // height: parent.height

        anchors {
            top: viewInfoPanel.bottom
            bottom: bottomPanel.top
            left: leftPanel.right
            right: rightPanel.left
        }
    }

    // ======================================================================================
    // ======================================================================================
    // ======================================================================================

    TopPanel {
        id: topPanel

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }

        height: appStateSettings.topPanelHeight
    }

    Rectangle {
        id: viewInfoPanel
        anchors {
            top: topPanel.bottom
            left: leftPanel.right
            right: rightPanel.left
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

    BottomPanel {
        id: bottomPanel

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        height: appStateSettings.bottomPanelHeight
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

        MouseArea {
            id: resizeHandle
            anchors {
                top: parent.top
                bottom: parent.bottom
                right: parent.right
            }

            width: 8

            cursorShape: Qt.SizeHorCursor

            onPressed: mouse => {
                leftPanel._pressWidth = leftPanel.width;
                leftPanel._pressMouseX = mouse.x;
            }

            onPositionChanged: mouse => {
                if (!pressed)
                    return;

                // Delta since press, in MouseArea-local coords
                const dx = mouse.x - leftPanel._pressMouseX;
                const newWidth = leftPanel._pressWidth + dx;

                appStateSettings.leftPanelWidth = newWidth;
            }

            Rectangle {
                anchors.fill: parent
                color: resizeHandle.containsMouse || resizeHandle.pressed ? "#80ffffff" : "transparent"
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
