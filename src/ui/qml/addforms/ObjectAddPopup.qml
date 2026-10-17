// qmllint disable unqualified

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls

import Tire 1.0
import "../components"

Rectangle {
    id: objectAddPopupComponent

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

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

        readonly property int itemsHeight: _units.scaled_24

        NpContextMenuButton {
            id: addSphereButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Box"
            enabled: false

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
            id: addConeButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Cone"
            enabled: false

            font: _fonts.label_accent
        }

        NpContextMenuButton {
            id: addTorusButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Torus"
            enabled: false
            font: _fonts.label_accent
        }

        NpContextMenuButton {
            id: addIcosphereButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Icosphere"
            enabled: true
            font: _fonts.label_accent

            onClicked: {
                Tired.scenegraph.scene.addIcosphere("OBJECT", "", Qt.vector3d(0, 0, 0), Qt.vector3d(0, 0, 0), Qt.vector3d(1, 1, 1), Qt.vector4d(1, 1, 1, 1), 0.5, 4);
            }
        }

        NpContextMenuButton {
            id: addIcosahedronButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Icosahedron"
            enabled: true
            font: _fonts.label_accent

            onClicked: {
                Tired.scenegraph.scene.addIcosahedron("OBJECT", "", Qt.vector3d(0, 0, 0), Qt.vector3d(0, 0, 0), Qt.vector3d(1, 1, 1), Qt.vector4d(1, 1, 1, 1), 0.5, 1);
            }
        }

        NpContextMenuButton {
            id: addDodecahedronButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Dodecahedron"
            enabled: true
            font: _fonts.label_accent

            onClicked: {
                Tired.scenegraph.scene.addDodecahedron("OBJECT", "", Qt.vector3d(0, 0, 0), Qt.vector3d(0, 0, 0), Qt.vector3d(1, 1, 1), Qt.vector4d(1, 1, 1, 1), 0.5, 1, 1);
            }
        }

        NpContextMenuButton {
            id: addPlaneButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Plane"
            enabled: true
            font: _fonts.label_accent

            onClicked: {
                Tired.scenegraph.scene.addPlane("OBJECT", "", Qt.vector3d(0, 0, 0), Qt.vector3d(0, 0, 0), Qt.vector3d(1, 1, 1), Qt.vector4d(1, 1, 1, 1), Qt.vector2d(0.5, 0.5), Qt.vector2d(1, 1));
            }
        }

        NpContextMenuButton {
            id: addDiskButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Disk"
            enabled: true
            font: _fonts.label_accent

            onClicked: {
                Tired.scenegraph.scene.addDisk("OBJECT", "", Qt.vector3d(0, 0, 0), Qt.vector3d(0, 0, 0), Qt.vector3d(1, 1, 1), Qt.vector4d(1, 1, 1, 1), 0.5, 0.0, 16, 4, 0.0, 2.0 * Math.PI);
            }
        }

        NpContextMenuButton {
            id: addTubeButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Tube"
            enabled: true
            font: _fonts.label_accent

            onClicked: {
                Tired.scenegraph.scene.addTube("OBJECT", "", Qt.vector3d(0, 0, 0), Qt.vector3d(0, 0, 0), Qt.vector3d(1, 1, 1), Qt.vector4d(1, 1, 1, 1), 0.5, 0.25, 0.5, 16, 8, 0.0, 2.0 * Math.PI);
            }
        }

        NpContextMenuButton {
            id: addCappedTubeButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Capped Tube"
            enabled: true
            font: _fonts.label_accent

            onClicked: {
                Tired.scenegraph.scene.addCappedTube("OBJECT", "", Qt.vector3d(0, 0, 0), Qt.vector3d(0, 0, 0), Qt.vector3d(1, 1, 1), Qt.vector4d(1, 1, 1, 1), 0.5, 0.25, 0.5, 16, 8, 1, 0.0, 2.0 * Math.PI);
            }
        }

        NpContextMenuButton {
            id: addRoundedBoxButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Rounded Box"
            enabled: true
            font: _fonts.label_accent

            onClicked: {
                Tired.scenegraph.scene.addRoundedBox("OBJECT", "", Qt.vector3d(0, 0, 0), Qt.vector3d(0, 0, 0), Qt.vector3d(1, 1, 1), Qt.vector4d(1, 1, 1, 1), 0.1, Qt.vector3d(0.4, 0.4, 0.4), 4, Qt.vector3d(1, 1, 1));
            }
        }

        NpContextMenuButton {
            id: addSphericalConeButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Spherical Cone"
            enabled: true
            font: _fonts.label_accent

            onClicked: {
                Tired.scenegraph.scene.addSphericalCone("OBJECT", "", Qt.vector3d(0, 0, 0), Qt.vector3d(0, 0, 0), Qt.vector3d(1, 1, 1), Qt.vector4d(1, 1, 1, 1), 0.5, 0.5, 16, 8, 4, 0.0, 2.0 * Math.PI);
            }
        }

        NpContextMenuButton {
            id: addTorusKnotButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Torus Knot"
            enabled: true
            font: _fonts.label_accent

            onClicked: {
                Tired.scenegraph.scene.addTorusKnot("OBJECT", "", Qt.vector3d(0, 0, 0), Qt.vector3d(0, 0, 0), Qt.vector3d(1, 1, 1), Qt.vector4d(1, 1, 1, 1), 2, 3, 8, 96);
            }
        }

        NpContextMenuButton {
            id: addSpringButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Spring"
            enabled: true
            font: _fonts.label_accent

            onClicked: {
                Tired.scenegraph.scene.addSpring("OBJECT", "", Qt.vector3d(0, 0, 0), Qt.vector3d(0, 0, 0), Qt.vector3d(1, 1, 1), Qt.vector4d(1, 1, 1, 1), 0.25, 1.0, 1.0, 8, 32, 0.0, 2.0 * Math.PI, 0.0, 4.0 * Math.PI);
            }
        }

        NpContextMenuButton {
            id: addTeapotButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Teapot"
            enabled: true
            font: _fonts.label_accent

            onClicked: {
                Tired.scenegraph.scene.addTeapot("OBJECT", "", Qt.vector3d(0, 0, 0), Qt.vector3d(0, 0, 0), Qt.vector3d(1, 1, 1), Qt.vector4d(1, 1, 1, 1), 8);
            }
        }
    }
}
