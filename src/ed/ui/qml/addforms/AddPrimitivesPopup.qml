// qmllint disable unqualified

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls

import Tire 1.0
import "../components"

Rectangle {
    id: addPrimitivesPopupComponent

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    height: visible ? addItemsButtonsLayout.implicitHeight : 0

    Behavior on height {
        NumberAnimation {
            duration: 50
        }
    }

    radius: _units.radiusEight
    color: _color.si_background_light

    ColumnLayout {
        id: addItemsButtonsLayout
        spacing: 0

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        readonly property int itemsHeight: 32

        NpContextMenuButton {
            id: addSphereButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Box"
            enabled: true

            topLeftRadius: _units.radiusEight
            topRightRadius: _units.radiusEight

            font: _fonts.label_accent
        }

        NpContextMenuButton {
            id: addBoxButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Sphere"
            enabled: false

            font: _fonts.label_accent
        }

        NpContextMenuButton {
            id: addCylinderButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Cylinder"
            enabled: false

            font: _fonts.label_accent
        }

        NpContextMenuButton {
            id: addCapsuleButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Capsule"
            enabled: false

            font: _fonts.label_accent
        }

        NpContextMenuButton {
            id: addFromFileButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "From file"
            enabled: true

            font: _fonts.label_accent
        }

        NpContextMenuButton {
            id: addImplicitButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight

            text: "Implicit surface"
            icon.source: "image://TiredImageProvider/star-alt-3-svgrepo-com.svg"

            enabled: false

            font: _fonts.label_accent
        }

        NpContextMenuButton {
            id: addBezierSurfaceButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Bezier surface"
            enabled: false

            font: _fonts.label_accent
        }

        NpContextMenuButton {
            id: addLandscapeButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Landscape"
            enabled: false

            font: _fonts.label_accent
        }

        NpContextMenuButton {
            id: addPolytopeButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Polytope"
            enabled: false

            font: _fonts.label_accent

            bottomLeftRadius: _units.radiusEight
            bottomRightRadius: _units.radiusEight
        }
    }
}
