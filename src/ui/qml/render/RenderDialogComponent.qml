// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: renderComponent

    anchors.fill: parent

    property real imageRatio: 0.75
    readonly property real minImageRatio: 0.2
    readonly property real maxImageRatio: 0.85

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    color: _color.si_background_light
    radius: _units.radiusEight

    function applyImageRatio(candidateRatio) {
        renderComponent.imageRatio = Math.min(Math.max(candidateRatio, renderComponent.minImageRatio), renderComponent.maxImageRatio);
    }

    Item {
        id: splitArea

        anchors {
            fill: parent
            margins: _units.scaled_4
        }

        clip: true

        RenderImageAreaComponent {
            id: imageArea

            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }

            width: (splitArea.width * renderComponent.imageRatio) - (splitHandle.width / 2)
        }

        Item {
            id: splitHandle

            width: _units.scaled_8

            anchors {
                top: parent.top
                bottom: parent.bottom
            }

            x: imageArea.width

            property real pressOffsetX: 0

            MouseArea {
                id: splitHandleArea

                anchors.fill: parent

                acceptedButtons: Qt.LeftButton
                cursorShape: Qt.SizeHorCursor

                onPressed: mouse => {
                    const cursorX = splitHandleArea.mapToItem(splitArea, mouse.x, 0).x;
                    const boundaryX = splitArea.width * renderComponent.imageRatio;

                    splitHandle.pressOffsetX = cursorX - boundaryX;
                }

                onPositionChanged: mouse => {
                    if (!pressed) {
                        return;
                    }

                    const cursorX = splitHandleArea.mapToItem(splitArea, mouse.x, 0).x;
                    const candidateRatio = (cursorX - splitHandle.pressOffsetX) / splitArea.width;

                    renderComponent.applyImageRatio(candidateRatio);
                }

                onDoubleClicked: {
                    if (splitArea.width <= 0) {
                        return;
                    }

                    const contentInset = 2 * _units.scaled_4;
                    const contentHeight = imageArea.height - contentInset;
                    const imageAspect = Tired.renderRequest.width / Tired.renderRequest.height;
                    const fittedImageAreaWidth = (contentHeight * imageAspect) + contentInset;

                    const candidateRatio = (fittedImageAreaWidth + (splitHandle.width / 2)) / splitArea.width;

                    renderComponent.applyImageRatio(candidateRatio);
                }
            }
        }

        RenderSettingsComponent {
            id: settingsArea

            anchors {
                left: splitHandle.right
                //leftMargin: _units.scaled_4
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }
        }
    }
}
