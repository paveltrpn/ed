import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import Qt5Compat.GraphicalEffects

import Tire 1.0

import "panels"

Item {
    id: mainWindow

    property bool greenExpanded: false

    // ======================================================================================
    // ==================== Render ==========================================================
    // ======================================================================================
    Render {
        id: render

        // width: parent.width
        // height: parent.height

        anchors {
            top: topPanel.bottom
            bottom: bottomPanel.top
            left: leftPanel.right
            right: rightPanel.left
        }

        Behavior on height {
            NumberAnimation {
                duration: 250
            }
        }

        Behavior on width {
            NumberAnimation {
                duration: 250
            }
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

        height: mainWindow.greenExpanded ? 128 : 64

        Behavior on height {
            NumberAnimation {
                duration: 250
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

        height: mainWindow.greenExpanded ? 128 : 64

        Behavior on height {
            NumberAnimation {
                duration: 250
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

        width: mainWindow.greenExpanded ? 128 : 64

        Behavior on width {
            NumberAnimation {
                duration: 250
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

        width: mainWindow.greenExpanded ? 128 : 64

        Behavior on width {
            NumberAnimation {
                duration: 250
            }
        }
    }

    Rectangle {
        anchors {
            // right: parent.right
            // bottom: parent.bottom

            centerIn: parent
        }

        radius: 8
        width: clickMessage.implicitWidth + 32
        height: 64

        color: "#4cff0000"

        Text {
            id: clickMessage
            anchors {
                fill: parent
            }

            text: "click me"
            color: "white"
            font: Qt.font({
                "pixelSize": 16,
                "weight": Font.ExtraBold,
                "family": "Monospace"
            })

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                mainWindow.greenExpanded = !mainWindow.greenExpanded;
                console.log(" i am clickable!!! ");
            }
        }
    }
}
