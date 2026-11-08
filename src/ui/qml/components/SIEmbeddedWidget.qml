// qmllint disable unqualified
// qmllint disable Quick.property-changes-parsed

import QtQuick

import Tire 1.0

Rectangle {
    id: siEmbeddedWidgetComponent

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    color: "transparent"

    property string title: ""

    property alias contentSource: widgetContentLoader.source

    signal aboutToClose
    signal internalItemReady

    property var internalItem: widgetContentLoader.item

    property Item dragBounds: null

    MouseArea {
        id: clickEater
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
        hoverEnabled: true
        propagateComposedEvents: false
        onPressed: mouse => mouse.accepted = true
        onReleased: mouse => mouse.accepted = true
        onClicked: mouse => mouse.accepted = true
        onDoubleClicked: mouse => mouse.accepted = true
        onPositionChanged: mouse => mouse.accepted = true
        onWheel: wheel => wheel.accepted = true
    }

    Rectangle {
        id: widgetBody

        anchors.fill: parent

        color: _color.si_background_light
        radius: _units.radiusEight

        border {
            width: _units.scaled_1
            color: _color.si_background_dark
        }

        Rectangle {
            id: titleBar

            anchors {
                top: parent.top
                topMargin: _units.scaled_4
                left: parent.left
                leftMargin: _units.scaled_4
                right: parent.right
                rightMargin: _units.scaled_4
            }

            height: _units.scaled_32
            color: _color.si_background_dark

            topLeftRadius: _units.radiusEight
            topRightRadius: _units.radiusEight

            MouseArea {
                id: headerArea

                anchors.fill: parent

                property point pressOrigin
                property point widgetOrigin

                onPressed: mouse => {
                    pressOrigin = mapToItem(null, mouse.x, mouse.y);
                    widgetOrigin = Qt.point(siEmbeddedWidgetComponent.x, siEmbeddedWidgetComponent.y);
                }

                onPositionChanged: mouse => {
                    if (!headerArea.pressed) {
                        return;
                    }

                    const position = mapToItem(null, mouse.x, mouse.y);

                    let newX = widgetOrigin.x + (position.x - pressOrigin.x);
                    let newY = widgetOrigin.y + (position.y - pressOrigin.y);

                    if (siEmbeddedWidgetComponent.dragBounds !== null) {
                        const bounds = siEmbeddedWidgetComponent.dragBounds.mapToItem(siEmbeddedWidgetComponent.parent, 0, 0);
                        const boundsRight = bounds.x + siEmbeddedWidgetComponent.dragBounds.width;
                        const boundsBottom = bounds.y + siEmbeddedWidgetComponent.dragBounds.height;

                        if (newX < bounds.x) {
                            newX = bounds.x;
                        }

                        if (newY < bounds.y) {
                            newY = bounds.y;
                        }

                        if (newX + siEmbeddedWidgetComponent.width > boundsRight) {
                            newX = boundsRight - siEmbeddedWidgetComponent.width;
                        }

                        if (newY + siEmbeddedWidgetComponent.height > boundsBottom) {
                            newY = boundsBottom - siEmbeddedWidgetComponent.height;
                        }
                    }

                    siEmbeddedWidgetComponent.x = newX;
                    siEmbeddedWidgetComponent.y = newY;
                }
            }

            Text {
                id: titleText

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                }

                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter

                font: _fonts.label
                color: "white"

                elide: Text.ElideRight

                text: siEmbeddedWidgetComponent.title
            }
        }

        Rectangle {
            id: innerComponentWrappoer

            anchors {
                top: titleBar.bottom
                right: widgetBody.right
                bottom: widgetBody.bottom
                left: widgetBody.left
                margins: _units.scaled_4
            }

            border {
                width: 1
                color: "black"
            }

            Loader {
                id: widgetContentLoader

                anchors {
                    fill: parent
                }

                onItemChanged: {
                    if (item !== null) {
                    }
                }

                onStatusChanged: {
                    switch (status) {
                    case Loader.Ready:
                        {
                            siEmbeddedWidgetComponent.internalItemReady();
                            break;
                        }
                    case Loader.Error:
                        {
                            break;
                        }
                    }
                }
            }
        }
    }
}
