import QtQuick 2.15
import QtQuick.Controls.impl 2.15
import Tire 1.0

Rectangle {
    id: dragAbleItemComponent

    readonly property var _fonts: Appearence.fonts
    readonly property var _gaps: Appearence.gaps
    readonly property var _radius: Appearence.radius
    readonly property var _color: Appearence.colors

    MouseArea {
        id: topMouseRegion
        anchors.fill: parent
        property var clickPos: Qt.point(1, 1)

        onPressed: mouse => {
                       clickPos = Qt.point(mouse.x, mouse.y)
                   }

        onPositionChanged: mouse => {
                               const delta = Qt.point(mouse.x - clickPos.x,
                                                      mouse.y - clickPos.y)
                               dragAbleItemComponent.x += delta.x
                               dragAbleItemComponent.y += delta.y
                           }
    }
}
