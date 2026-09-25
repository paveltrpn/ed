// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: selectedInfoComponent

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    bottomLeftRadius: _units.radiusEight
    bottomRightRadius: _units.radiusEight

    color: _color.si_background_light

    // implicitHeight: selectedInfoLayout.implicitHeight + _units.full

    Item {
        id: objectInfoArea

        anchors {
            fill: parent
        }

        Connections {
            target: Tired.scenegraph.scene
            function onSelectedObjectChanged(object) {
                if (object !== null) {
                    switch (object.type) {
                    case 0:
                        {
                            objectInfoLoader.source = "";
                            break;
                        }
                    case 1:
                        {
                            objectInfoLoader.source = "./BoxInfoWidget.qml";
                            break;
                        }
                    case 2:
                        {
                            objectInfoLoader.source = "./SphereInfoWidget.qml";
                            break;
                        }
                    case 3:
                        {
                            objectInfoLoader.source = "./CylinderInfoWidget.qml";
                            break;
                        }
                    case 4:
                        {
                            objectInfoLoader.source = "./CapsuleInfoWidget.qml";
                            break;
                        }
                    case 5:
                        {
                            objectInfoLoader.source = "./ConeInfoWidget.qml";
                            break;
                        }
                    case 6:
                        {
                            objectInfoLoader.source = "./TorusInfoWidget.qml";
                            break;
                        }
                    }
                }
            }
        }

        states: [
            State {
                name: "no_object_selected"
                when: !Tired.scenegraph.scene.isAnyObjectSelected
                PropertyChanges {
                    noObjectSelectedDummy.visible: true
                    someObjectSelectedWrapper.visible: false
                }
            },
            State {
                name: "some_object_selected"
                when: Tired.scenegraph.scene.isAnyObjectSelected
                PropertyChanges {
                    noObjectSelectedDummy.visible: false
                    someObjectSelectedWrapper.visible: true
                }
            }
        ]

        Text {
            id: noObjectSelectedDummy

            anchors {
                fill: parent
            }

            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter

            font: _fonts.text_body_accent

            color: _color.si_background_dark

            text: "No objects selected"
        }

        Item {
            id: someObjectSelectedWrapper

            anchors {
                fill: parent
            }

            Loader {
                id: objectInfoLoader

                anchors {
                    fill: parent
                }
            }
        }
    }
}
