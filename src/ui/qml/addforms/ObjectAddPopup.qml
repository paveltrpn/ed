// qmllint disable unqualified

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls

import Tire 1.0
import "../components"

Rectangle {
    id: objectAddPopupComponent

    // SceneObjectTypeEnum values (enum is not QML-registered):
    // BOX=1, SPHERE=2, CYLINDER=3, CAPSULE=4, CONE=5, TORUS=6, ICOSPHERE=12, ICOSAHEDRON=13,
    // DODECAHEDRON=14, PLANE=15, DISK=16, TUBE=17, CAPPED_TUBE=18, ROUNDED_BOX=19,
    // SPHERICAL_CONE=20, TORUS_KNOT=21, SPRING=22, TEAPOT=23

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
                Tired.scenegraph.scene.add({ "type": 12, "name": "OBJECT", "position": [0.0, 0.0, 0.0], "orientation": [0.0, 0.0, 0.0], "scale": [1.0, 1.0, 1.0], "color": [1.0, 1.0, 1.0, 1.0], "derived": { "radius": 0.5, "segments": 4 } });
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
                Tired.scenegraph.scene.add({ "type": 13, "name": "OBJECT", "position": [0.0, 0.0, 0.0], "orientation": [0.0, 0.0, 0.0], "scale": [1.0, 1.0, 1.0], "color": [1.0, 1.0, 1.0, 1.0], "derived": { "radius": 0.5, "segments": 1 } });
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
                Tired.scenegraph.scene.add({ "type": 14, "name": "OBJECT", "position": [0.0, 0.0, 0.0], "orientation": [0.0, 0.0, 0.0], "scale": [1.0, 1.0, 1.0], "color": [1.0, 1.0, 1.0, 1.0], "derived": { "radius": 0.5, "segments": 1, "rings": 1 } });
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
                Tired.scenegraph.scene.add({ "type": 15, "name": "OBJECT", "position": [0.0, 0.0, 0.0], "orientation": [0.0, 0.0, 0.0], "scale": [1.0, 1.0, 1.0], "color": [1.0, 1.0, 1.0, 1.0], "derived": { "sizeX": 0.5, "sizeY": 0.5, "segmentsX": 1, "segmentsY": 1 } });
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
                Tired.scenegraph.scene.add({ "type": 16, "name": "OBJECT", "position": [0.0, 0.0, 0.0], "orientation": [0.0, 0.0, 0.0], "scale": [1.0, 1.0, 1.0], "color": [1.0, 1.0, 1.0, 1.0], "derived": { "radius": 0.5, "innerRadius": 0.0, "slices": 16, "rings": 4, "start": 0.0, "sweep": 2.0 * Math.PI } });
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
                Tired.scenegraph.scene.add({ "type": 17, "name": "OBJECT", "position": [0.0, 0.0, 0.0], "orientation": [0.0, 0.0, 0.0], "scale": [1.0, 1.0, 1.0], "color": [1.0, 1.0, 1.0, 1.0], "derived": { "radius": 0.5, "innerRadius": 0.25, "size": 0.5, "slices": 16, "segments": 8, "start": 0.0, "sweep": 2.0 * Math.PI } });
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
                Tired.scenegraph.scene.add({ "type": 18, "name": "OBJECT", "position": [0.0, 0.0, 0.0], "orientation": [0.0, 0.0, 0.0], "scale": [1.0, 1.0, 1.0], "color": [1.0, 1.0, 1.0, 1.0], "derived": { "radius": 0.5, "innerRadius": 0.25, "size": 0.5, "slices": 16, "segments": 8, "rings": 1, "start": 0.0, "sweep": 2.0 * Math.PI } });
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
                Tired.scenegraph.scene.add({ "type": 19, "name": "OBJECT", "position": [0.0, 0.0, 0.0], "orientation": [0.0, 0.0, 0.0], "scale": [1.0, 1.0, 1.0], "color": [1.0, 1.0, 1.0, 1.0], "derived": { "radius": 0.1, "sizeX": 0.4, "sizeY": 0.4, "sizeZ": 0.4, "slices": 4, "segmentsX": 1, "segmentsY": 1, "segmentsZ": 1 } });
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
                Tired.scenegraph.scene.add({ "type": 20, "name": "OBJECT", "position": [0.0, 0.0, 0.0], "orientation": [0.0, 0.0, 0.0], "scale": [1.0, 1.0, 1.0], "color": [1.0, 1.0, 1.0, 1.0], "derived": { "radius": 0.5, "size": 0.5, "slices": 16, "segments": 8, "rings": 4, "start": 0.0, "sweep": 2.0 * Math.PI } });
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
                Tired.scenegraph.scene.add({ "type": 21, "name": "OBJECT", "position": [0.0, 0.0, 0.0], "orientation": [0.0, 0.0, 0.0], "scale": [1.0, 1.0, 1.0], "color": [1.0, 1.0, 1.0, 1.0], "derived": { "p": 2, "q": 3, "slices": 8, "segments": 96 } });
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
                Tired.scenegraph.scene.add({ "type": 22, "name": "OBJECT", "position": [0.0, 0.0, 0.0], "orientation": [0.0, 0.0, 0.0], "scale": [1.0, 1.0, 1.0], "color": [1.0, 1.0, 1.0, 1.0], "derived": { "minor": 0.25, "major": 1.0, "size": 1.0, "slices": 8, "segments": 32, "minorStart": 0.0, "minorSweep": 2.0 * Math.PI, "majorStart": 0.0, "majorSweep": 4.0 * Math.PI } });
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
                Tired.scenegraph.scene.add({ "type": 23, "name": "OBJECT", "position": [0.0, 0.0, 0.0], "orientation": [0.0, 0.0, 0.0], "scale": [1.0, 1.0, 1.0], "color": [1.0, 1.0, 1.0, 1.0], "derived": { "segments": 8 } });
            }
        }
    }
}
