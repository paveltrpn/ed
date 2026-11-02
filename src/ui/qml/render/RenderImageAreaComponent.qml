// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Item {
    id: renderImageAreaItem

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    width: renderedImageArea.width
    height: renderedImageArea.height

    signal close

    Rectangle {
        id: renderedImageArea

        width: Tired.renderRequest.width
        height: Tired.renderRequest.height

        color: "transparent"

        RenderImageItem {
            anchors.fill: parent
            image: Tired.renderRequest.renderedImage
        }
    }
}
