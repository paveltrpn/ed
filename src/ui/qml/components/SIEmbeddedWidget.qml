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

    property int minimumWidth: 320
    property int minimumHeight: 240
    property int maximumWidth: 1024
    property int maximumHeight: 768

    property point _pressOrigin
    property rect _resizeGeometry

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

    MouseArea {
        id: topResizeHandle

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }

        height: _units.scaled_4
        cursorShape: Qt.SizeVerCursor

        onPressed: mouse => {
            siEmbeddedWidgetComponent._pressOrigin = mapToItem(null, mouse.x, mouse.y);
            siEmbeddedWidgetComponent._resizeGeometry = Qt.rect(siEmbeddedWidgetComponent.x, siEmbeddedWidgetComponent.y, siEmbeddedWidgetComponent.width, siEmbeddedWidgetComponent.height);
        }

        onPositionChanged: mouse => {
            if (!pressed) {
                return;
            }

            const position = mapToItem(null, mouse.x, mouse.y);

            let newHeight = siEmbeddedWidgetComponent._resizeGeometry.height - (position.y - siEmbeddedWidgetComponent._pressOrigin.y);

            if (newHeight < siEmbeddedWidgetComponent.minimumHeight) {
                newHeight = siEmbeddedWidgetComponent.minimumHeight;
            }

            if (newHeight > siEmbeddedWidgetComponent.maximumHeight) {
                newHeight = siEmbeddedWidgetComponent.maximumHeight;
            }

            let newY = siEmbeddedWidgetComponent._resizeGeometry.y + (siEmbeddedWidgetComponent._resizeGeometry.height - newHeight);

            if (siEmbeddedWidgetComponent.dragBounds !== null) {
                const bounds = siEmbeddedWidgetComponent.dragBounds.mapToItem(siEmbeddedWidgetComponent.parent, 0, 0);

                if (newY < bounds.y) {
                    newY = bounds.y;
                    newHeight = siEmbeddedWidgetComponent._resizeGeometry.y + siEmbeddedWidgetComponent._resizeGeometry.height - newY;
                }
            }

            if (newHeight < siEmbeddedWidgetComponent.minimumHeight) {
                newHeight = siEmbeddedWidgetComponent.minimumHeight;
            }

            siEmbeddedWidgetComponent.y = newY;
            siEmbeddedWidgetComponent.height = newHeight;
        }
    }

    MouseArea {
        id: bottomResizeHandle

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        height: _units.scaled_4
        cursorShape: Qt.SizeVerCursor

        onPressed: mouse => {
            siEmbeddedWidgetComponent._pressOrigin = mapToItem(null, mouse.x, mouse.y);
            siEmbeddedWidgetComponent._resizeGeometry = Qt.rect(siEmbeddedWidgetComponent.x, siEmbeddedWidgetComponent.y, siEmbeddedWidgetComponent.width, siEmbeddedWidgetComponent.height);
        }

        onPositionChanged: mouse => {
            if (!pressed) {
                return;
            }

            const position = mapToItem(null, mouse.x, mouse.y);

            let newHeight = siEmbeddedWidgetComponent._resizeGeometry.height + (position.y - siEmbeddedWidgetComponent._pressOrigin.y);

            if (newHeight < siEmbeddedWidgetComponent.minimumHeight) {
                newHeight = siEmbeddedWidgetComponent.minimumHeight;
            }

            if (newHeight > siEmbeddedWidgetComponent.maximumHeight) {
                newHeight = siEmbeddedWidgetComponent.maximumHeight;
            }

            if (siEmbeddedWidgetComponent.dragBounds !== null) {
                const bounds = siEmbeddedWidgetComponent.dragBounds.mapToItem(siEmbeddedWidgetComponent.parent, 0, 0);
                const boundsBottom = bounds.y + siEmbeddedWidgetComponent.dragBounds.height;

                if (siEmbeddedWidgetComponent._resizeGeometry.y + newHeight > boundsBottom) {
                    newHeight = boundsBottom - siEmbeddedWidgetComponent._resizeGeometry.y;
                }
            }

            if (newHeight < siEmbeddedWidgetComponent.minimumHeight) {
                newHeight = siEmbeddedWidgetComponent.minimumHeight;
            }

            siEmbeddedWidgetComponent.y = siEmbeddedWidgetComponent._resizeGeometry.y;
            siEmbeddedWidgetComponent.height = newHeight;
        }
    }

    MouseArea {
        id: leftResizeHandle

        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
        }

        width: _units.scaled_4
        cursorShape: Qt.SizeHorCursor

        onPressed: mouse => {
            siEmbeddedWidgetComponent._pressOrigin = mapToItem(null, mouse.x, mouse.y);
            siEmbeddedWidgetComponent._resizeGeometry = Qt.rect(siEmbeddedWidgetComponent.x, siEmbeddedWidgetComponent.y, siEmbeddedWidgetComponent.width, siEmbeddedWidgetComponent.height);
        }

        onPositionChanged: mouse => {
            if (!pressed) {
                return;
            }

            const position = mapToItem(null, mouse.x, mouse.y);

            let newWidth = siEmbeddedWidgetComponent._resizeGeometry.width - (position.x - siEmbeddedWidgetComponent._pressOrigin.x);

            if (newWidth < siEmbeddedWidgetComponent.minimumWidth) {
                newWidth = siEmbeddedWidgetComponent.minimumWidth;
            }

            if (newWidth > siEmbeddedWidgetComponent.maximumWidth) {
                newWidth = siEmbeddedWidgetComponent.maximumWidth;
            }

            let newX = siEmbeddedWidgetComponent._resizeGeometry.x + (siEmbeddedWidgetComponent._resizeGeometry.width - newWidth);

            if (siEmbeddedWidgetComponent.dragBounds !== null) {
                const bounds = siEmbeddedWidgetComponent.dragBounds.mapToItem(siEmbeddedWidgetComponent.parent, 0, 0);

                if (newX < bounds.x) {
                    newX = bounds.x;
                    newWidth = siEmbeddedWidgetComponent._resizeGeometry.x + siEmbeddedWidgetComponent._resizeGeometry.width - newX;
                }
            }

            if (newWidth < siEmbeddedWidgetComponent.minimumWidth) {
                newWidth = siEmbeddedWidgetComponent.minimumWidth;
            }

            siEmbeddedWidgetComponent.x = newX;
            siEmbeddedWidgetComponent.width = newWidth;
        }
    }

    MouseArea {
        id: rightResizeHandle

        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }

        width: _units.scaled_4
        cursorShape: Qt.SizeHorCursor

        onPressed: mouse => {
            siEmbeddedWidgetComponent._pressOrigin = mapToItem(null, mouse.x, mouse.y);
            siEmbeddedWidgetComponent._resizeGeometry = Qt.rect(siEmbeddedWidgetComponent.x, siEmbeddedWidgetComponent.y, siEmbeddedWidgetComponent.width, siEmbeddedWidgetComponent.height);
        }

        onPositionChanged: mouse => {
            if (!pressed) {
                return;
            }

            const position = mapToItem(null, mouse.x, mouse.y);

            let newWidth = siEmbeddedWidgetComponent._resizeGeometry.width + (position.x - siEmbeddedWidgetComponent._pressOrigin.x);

            if (newWidth < siEmbeddedWidgetComponent.minimumWidth) {
                newWidth = siEmbeddedWidgetComponent.minimumWidth;
            }

            if (newWidth > siEmbeddedWidgetComponent.maximumWidth) {
                newWidth = siEmbeddedWidgetComponent.maximumWidth;
            }

            if (siEmbeddedWidgetComponent.dragBounds !== null) {
                const bounds = siEmbeddedWidgetComponent.dragBounds.mapToItem(siEmbeddedWidgetComponent.parent, 0, 0);
                const boundsRight = bounds.x + siEmbeddedWidgetComponent.dragBounds.width;

                if (siEmbeddedWidgetComponent._resizeGeometry.x + newWidth > boundsRight) {
                    newWidth = boundsRight - siEmbeddedWidgetComponent._resizeGeometry.x;
                }
            }

            if (newWidth < siEmbeddedWidgetComponent.minimumWidth) {
                newWidth = siEmbeddedWidgetComponent.minimumWidth;
            }

            siEmbeddedWidgetComponent.x = siEmbeddedWidgetComponent._resizeGeometry.x;
            siEmbeddedWidgetComponent.width = newWidth;
        }
    }
}
