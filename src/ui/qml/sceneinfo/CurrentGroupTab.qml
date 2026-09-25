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

    property var selectedObject: null

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
                selectedInfoComponent.selectedObject = object;

                if (object !== null) {
                    let wantedSource = "";

                    switch (object.type) {
                    case 0:
                        {
                            wantedSource = "";
                            break;
                        }
                    case 1:
                        {
                            wantedSource = "./BoxInfoWidget.qml";
                            break;
                        }
                    case 2:
                        {
                            wantedSource = "./SphereInfoWidget.qml";
                            break;
                        }
                    case 3:
                        {
                            wantedSource = "./CylinderInfoWidget.qml";
                            break;
                        }
                    case 4:
                        {
                            wantedSource = "./CapsuleInfoWidget.qml";
                            break;
                        }
                    case 5:
                        {
                            wantedSource = "./ConeInfoWidget.qml";
                            break;
                        }
                    case 6:
                        {
                            wantedSource = "./TorusInfoWidget.qml";
                            break;
                        }
                    }

                    if (objectInfoLoader.source !== wantedSource) {
                        objectInfoLoader.source = wantedSource;
                    } else if (objectInfoLoader.status === Loader.Ready) {
                        objectInfoLoader.item.object = object;
                    }
                } else if (objectInfoLoader.status === Loader.Ready) {
                    objectInfoLoader.item.object = null;
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

                onLoaded: {
                    if (objectInfoLoader.item !== null && selectedInfoComponent.selectedObject !== null) {
                        objectInfoLoader.item.object = selectedInfoComponent.selectedObject;
                    }
                }
            }
        }
    }
}
