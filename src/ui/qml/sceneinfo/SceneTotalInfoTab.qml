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

        Text {
            id: listHederText

            anchors {
                left: parent.left
                leftMargin: _units.scaled_4
            }

            height: 32
            width: implicitWidth

            color: _color.si_text_dark

            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter

            font: _fonts.text_body_accent

            text: " Objects list:"
        }

        ListView {
            id: objectsListView

            spacing: 0

            anchors {
                top: listHederText.bottom
                left: parent.left
                leftMargin: _units.full * 2
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
                        left: parent.left
                        leftMargin: _units.scaled_4
                        top: parent.top
                        bottom: parent.bottom
                        right: removeBtn.left
                        rightMargin: _units.scaled_4
                    }

                    color: _color.si_background_light

                    // border {
                    //     color: _color.si_background_dark
                    //     width: _units.scaled_1
                    // }

                    states: [
                        State {
                            name: "hovered"
                            when: listItemMouseArea.containsMouse && (model.object.uid !== Tired.scenegraph.scene.selectedObjectUid)
                            PropertyChanges {
                                frame.color: _color.si_background_light_faded
                            }
                        },
                        State {
                            name: "seleced"
                            when: (model.object.uid === Tired.scenegraph.scene.selectedObjectUid)
                            PropertyChanges {
                                frame.color: _color.si_button_bg_blue
                                objectDataText.color: _color.si_text_light
                            }
                        }
                    ]

                    MouseArea {
                        id: listItemMouseArea

                        anchors {
                            fill: parent
                            topMargin: _units.scaled_2
                            bottomMargin: _units.scaled_2
                        }

                        hoverEnabled: true

                        onClicked: {
                            if (Tired.scenegraph.scene.selectedObjectUid === model.object.uid) {
                                Tired.scenegraph.scene.selectedObjectUid = "{00000000-0000-0000-0000-000000000000}";
                                return;
                            }

                            Tired.scenegraph.scene.selectedObjectUid = model.object.uid;
                        }
                    }

                    Text {
                        id: objectDataText

                        anchors {
                            left: parent.left
                            leftMargin: _units.scaled_4
                            top: parent.top
                            bottom: parent.bottom
                            right: parent.right
                        }

                        color: _color.si_text_dark

                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHLeft

                        font: _fonts.label_accent

                        elide: Text.ElideRight

                        text: {
                            const name = model.object.name;
                            const uid = model.object.uid;
                            return `name: ${name} uid: ${uid}`;
                        }
                    }
                }

                SIButton {
                    id: removeBtn

                    anchors {
                        right: parent.right
                        rightMargin: _units.scaled_4
                        top: parent.top
                        topMargin: _units.scaled_2
                        bottom: parent.bottom
                        bottomMargin: _units.scaled_2
                    }

                    width: 32

                    buttonLabel: "D"

                    onClicked: {
                        Tired.scenegraph.scene.removeObject(model.object.uid);
                    }
                }
            }
        }
    }
}
