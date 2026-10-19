// qmllint disable unqualified

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls

import Tire 1.0
import "../components"

Rectangle {
    id: objectAddPopupComponent

    // SceneObjectTypeEnum values (enum is not QML-registered):
    // BOX=1, SPHERE=2, CYLINDER=3, CAPSULE=4, CONE=5, TORUS=6, ICOSPHERE=13, ICOSAHEDRON=14,
    // DODECAHEDRON=15, PLANE=16, DISK=17, TUBE=18, CAPPED_TUBE=19, ROUNDED_BOX=20,
    // SPHERICAL_CONE=21, TORUS_KNOT=22, SPRING=23, TEAPOT=24

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
            id: addBoxButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Sphere"
            enabled: true

            font: _fonts.label_accent

            onClicked: {
                Tired.scenegraph.scene.addObject({ "type": 2 });
            }
        }


        NpContextMenuButton {
            id: addSphereButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Box"
            enabled: true

            topLeftRadius: _units.radiusEight
            topRightRadius: _units.radiusEight

            font: _fonts.label_accent

            onClicked: {
                Tired.scenegraph.scene.addObject({ "type": 1 });
            }
        }

        NpContextMenuButton {
            id: addCylinderButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Cylinder"
            enabled: true

            font: _fonts.label_accent

            onClicked: {
                Tired.scenegraph.scene.addObject({ "type": 3 });
            }
        }

        NpContextMenuButton {
            id: addCapsuleButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Capsule"
            enabled: true

            font: _fonts.label_accent

            onClicked: {
                Tired.scenegraph.scene.addObject({ "type": 4 });
            }
        }

        NpContextMenuButton {
            id: addConeButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Cone"
            enabled: true

            font: _fonts.label_accent

            onClicked: {
                Tired.scenegraph.scene.addObject({ "type": 5 });
            }
        }

        NpContextMenuButton {
            id: addTorusButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Torus"
            enabled: true
            font: _fonts.label_accent

            onClicked: {
                Tired.scenegraph.scene.addObject({ "type": 6 });
            }
        }

        NpContextMenuButton {
            id: addIcosphereButton
            Layout.fillWidth: true
            Layout.preferredHeight: addItemsButtonsLayout.itemsHeight
            text: "Icosphere"
            enabled: true
            font: _fonts.label_accent

            onClicked: {
                Tired.scenegraph.scene.addObject({ "type": 13 });
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
                Tired.scenegraph.scene.addObject({ "type": 14 });
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
                Tired.scenegraph.scene.addObject({ "type": 15 });
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
                Tired.scenegraph.scene.addObject({ "type": 16 });
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
                Tired.scenegraph.scene.addObject({ "type": 17 });
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
                Tired.scenegraph.scene.addObject({ "type": 18 });
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
                Tired.scenegraph.scene.addObject({ "type": 19 });
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
                Tired.scenegraph.scene.addObject({ "type": 20 });
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
                Tired.scenegraph.scene.addObject({ "type": 21 });
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
                Tired.scenegraph.scene.addObject({ "type": 22 });
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
                Tired.scenegraph.scene.addObject({ "type": 23 });
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
                Tired.scenegraph.scene.addObject({ "type": 24 });
            }
        }
    }
}
