// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: sceneTotalInfoComponent

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    bottomLeftRadius: _units.radiusEight
    bottomRightRadius: _units.radiusEight

    color: _color.si_background_light

    height: sceneInfoAreaWrapper.height + _units.full

    Item {
        id: sceneInfoAreaWrapper

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        height: objectsListView.height

        ListView {
            id: objectsListView

            anchors {
                top: parent.top
                left: parent.left
                leftMargin: _units.half
                right: parent.right
                rightMargin: _units.half
            }

            height: contentHeight

            model: Tired.scenegraph.scene.objects

            delegate: Item {
                width: objectsListView.width
                height: _units.scaled_28

                Rectangle {
                    id: frame

                    anchors {
                        fill: parent
                        topMargin: _units.scaled_2
                        bottomMargin: _units.scaled_2
                    }

                    color: _color.si_background_light

                    border {
                        color: _color.si_background_dark
                        width: _units.scaled_1
                    }

                    Text {
                        id: objectDataText

                        anchors {
                            fill: parent
                            leftMargin: _units.scaled_4
                        }

                        color: _color.si_text_dark

                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter

                        font: _fonts.label_accent

                        elide: Text.ElideRight

                        text: {
                            const name = model.object.name;
                            const uid = model.object.uid;
                            return `${name} ${uid}`;
                        }
                    }
                }
            }
        }
    }
}
