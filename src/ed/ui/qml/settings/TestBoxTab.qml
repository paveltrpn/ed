// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: testBoxComponent

    readonly property var _fonts: Appearence.fonts
    readonly property var _gaps: Appearence.gaps
    readonly property var _radius: Appearence.radius
    readonly property var _color: Appearence.colors

    bottomLeftRadius: 2
    bottomRightRadius: 2

    color: _color.popup_background

    implicitHeight: testBoxSettingsLayout.implicitHeight + _gaps.full

    ColumnLayout {
        id: testBoxSettingsLayout

        spacing: _gaps.half

        anchors {
            left: parent.left
            right: parent.right
        }

        Item {
            id: spacer1
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: _gaps.half
        }

        Loader {
            id: boxOriginField
            sourceComponent: testBox3ComponentParameter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 24

            onLoaded: {
                item.parameterlabel = "Box origin";
                item.parameterValueX = Tired.scenegraph.testbox.boxOrigin.x;
                item.parameterValueY = Tired.scenegraph.testbox.boxOrigin.y;
                item.parameterValueZ = Tired.scenegraph.testbox.boxOrigin.z;
            }

            Connections {
                target: boxOriginField.item
                function onParamValueChanged(valueX, valueY, valueZ) {
                    Tired.scenegraph.testbox.boxOrigin = Qt.vector3d(valueX, valueY, valueZ);
                }
            }
        }

        Loader {
            id: boxSizeField
            sourceComponent: testBoxParameter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 24

            onLoaded: {
                item.parameterlabel = "Box size";
                item.parameterValue = Tired.scenegraph.testbox.boxSize;
            }

            Connections {
                target: boxSizeField.item
                function onParamValueChanged(value) {
                    Tired.scenegraph.testbox.boxSize = value;
                }
            }
        }

        Loader {
            id: boxAxisField
            sourceComponent: testBox3ComponentParameter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 24

            onLoaded: {
                item.parameterlabel = "Box axis";
                item.parameterValueX = Tired.scenegraph.testbox.boxAxis.x;
                item.parameterValueY = Tired.scenegraph.testbox.boxAxis.y;
                item.parameterValueZ = Tired.scenegraph.testbox.boxAxis.z;
            }

            Connections {
                target: boxAxisField.item
                function onParamValueChanged(valueX, valueY, valueZ) {
                    Tired.scenegraph.testbox.boxAxis = Qt.vector3d(valueX, valueY, valueZ);
                }
            }
        }

        Loader {
            id: boxAnglField
            sourceComponent: testBoxParameter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 24

            onLoaded: {
                item.parameterlabel = "Box angle";
                item.parameterValue = Tired.scenegraph.testbox.boxAngl;
            }

            Connections {
                target: boxAnglField.item
                function onParamValueChanged(value) {
                    Tired.scenegraph.testbox.boxAngl = value;
                }
            }
        }

        Loader {
            id: lightOriginField
            sourceComponent: testBox3ComponentParameter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 24

            onLoaded: {
                item.parameterlabel = "Light origin";
                item.parameterValueX = Tired.scenegraph.testbox.lightOrigin.x;
                item.parameterValueY = Tired.scenegraph.testbox.lightOrigin.y;
                item.parameterValueZ = Tired.scenegraph.testbox.lightOrigin.z;
            }

            Connections {
                target: lightOriginField.item
                function onParamValueChanged(valueX, valueY, valueZ) {
                    Tired.scenegraph.testbox.lightOrigin = Qt.vector3d(valueX, valueY, valueZ);
                }
            }
        }
    }

    Component {
        id: testBoxParameter

        Item {
            id: textBoxParameterWrapper

            anchors.fill: parent

            property string parameterlabel
            property real parameterValue

            signal paramValueChanged(value: real)

            Text {
                id: paramLabel
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                    leftMargin: 8
                    right: paramValue.left
                }

                text: textBoxParameterWrapper.parameterlabel

                verticalAlignment: Text.AlignVCenter

                color: _color.text_contrast
                font: _fonts.label

                elide: Text.ElideRight
            }

            TiredTextInput {
                id: paramValue

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    right: parent.right
                    rightMargin: 8
                }

                width: 80

                text: textBoxParameterWrapper.parameterValue.toFixed(3)

                onTextChanged: {
                    textBoxParameterWrapper.paramValueChanged(parseFloat(text));
                }
            }
        }
    }

    Component {
        id: testBox3ComponentParameter

        Item {
            id: textBox3ComponentParameterWrapper

            anchors.fill: parent

            property string parameterlabel
            property real parameterValueX
            property real parameterValueY
            property real parameterValueZ

            signal paramValueChanged(valueX: real, valueY: real, valueZ: real)

            Text {
                id: paramLabel
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                    leftMargin: 8
                    right: paramValueX.left
                }

                text: textBox3ComponentParameterWrapper.parameterlabel

                verticalAlignment: Text.AlignVCenter

                color: _color.text_contrast
                font: _fonts.label

                elide: Text.ElideRight
            }

            TiredTextInput {
                id: paramValueZ

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    right: parent.right
                    rightMargin: 8
                }

                width: 80

                text: textBox3ComponentParameterWrapper.parameterValueZ.toFixed(3)

                onTextChanged: {
                    const valueX = parseFloat(paramValueX.text)
                    const valueY = parseFloat(paramValueY.text)
                    const valueZ = parseFloat(paramValueZ.text)
                    textBox3ComponentParameterWrapper.paramValueChanged(valueX, valueY, valueZ);
                }
            }

            TiredTextInput {
                id: paramValueY

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    right: paramValueZ.left
                    rightMargin: 8
                }

                width: 80

                text: textBox3ComponentParameterWrapper.parameterValueY.toFixed(3)

                onTextChanged: {
                    const valueX = parseFloat(paramValueX.text)
                    const valueY = parseFloat(paramValueY.text)
                    const valueZ = parseFloat(paramValueZ.text)
                    textBox3ComponentParameterWrapper.paramValueChanged(valueX, valueY, valueZ);
                }
            }

            TiredTextInput {
                id: paramValueX

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    right: paramValueY.left
                    rightMargin: 8
                }

                width: 80

                text: textBox3ComponentParameterWrapper.parameterValueX.toFixed(3)

                onTextChanged: {
                    const valueX = parseFloat(paramValueX.text)
                    const valueY = parseFloat(paramValueY.text)
                    const valueZ = parseFloat(paramValueZ.text)
                    textBox3ComponentParameterWrapper.paramValueChanged(valueX, valueY, valueZ);
                }
            }
        }
    }
}
