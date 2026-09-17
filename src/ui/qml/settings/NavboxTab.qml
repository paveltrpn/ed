// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: navboxComponent

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    bottomLeftRadius: _units.radiusEight
    bottomRightRadius: _units.radiusEight

    color: _color.si_background_light

    implicitHeight: navboxSettingsLayout.implicitHeight + _units.full

    ColumnLayout {
        id: navboxSettingsLayout

        spacing: _units.half

        anchors {
            left: parent.left
            right: parent.right
        }

        Item {
            id: spacer1
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: _units.half
        }

        Loader {
            id: boxOriginField
            sourceComponent: navbox3ComponentParameter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 24

            onLoaded: {
                boxOriginField.item.parameterlabel = "Box origin";
                const value = Tired.scenegraph.navbox.boxOrigin;
                boxOriginField.item.parameterValueX = value.x;
                boxOriginField.item.parameterValueY = value.y;
                boxOriginField.item.parameterValueZ = value.z;
            }

            Connections {
                target: boxOriginField.item
                function onParamValueChanged(valueX, valueY, valueZ) {
                    Tired.scenegraph.navbox.boxOrigin = Qt.vector3d(valueX, valueY, valueZ);
                }
            }
        }

        Loader {
            id: boxSizeField
            sourceComponent: navboxParameter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 24

            onLoaded: {
                boxSizeField.item.parameterlabel = "Box size";
                boxSizeField.item.parameterValue = Tired.scenegraph.navbox.boxSize;
            }

            Connections {
                target: boxSizeField.item
                function onParamValueChanged(value) {
                    Tired.scenegraph.navbox.boxSize = value;
                }
            }
        }

        Loader {
            id: boxAxisField
            sourceComponent: navbox3ComponentParameter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 24

            onLoaded: {
                boxAxisField.item.parameterlabel = "Box axis";
                const value = Tired.scenegraph.navbox.boxAxis;
                boxAxisField.item.parameterValueX = value.x;
                boxAxisField.item.parameterValueY = value.y;
                boxAxisField.item.parameterValueZ = value.z;
            }

            Connections {
                target: boxAxisField.item
                function onParamValueChanged(valueX, valueY, valueZ) {
                    Tired.scenegraph.navbox.boxAxis = Qt.vector3d(valueX, valueY, valueZ);
                }
            }
        }

        Loader {
            id: boxAnglField
            sourceComponent: navboxParameter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 24

            onLoaded: {
                item.parameterlabel = "Box angle";
                item.parameterValue = Tired.scenegraph.navbox.boxAngl;
            }

            Connections {
                target: boxAnglField.item
                function onParamValueChanged(value) {
                    Tired.scenegraph.navbox.boxAngl = value;
                }
            }
        }

        Loader {
            id: lightOriginField
            sourceComponent: navbox3ComponentParameter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 24

            onLoaded: {
                item.parameterlabel = "Light origin";
                const value = Tired.scenegraph.navbox.lightOrigin;
                item.parameterValueX = value.x;
                item.parameterValueY = value.y;
                item.parameterValueZ = value.z;
            }

            Connections {
                target: lightOriginField.item
                function onParamValueChanged(valueX, valueY, valueZ) {
                    Tired.scenegraph.navbox.lightOrigin = Qt.vector3d(valueX, valueY, valueZ);
                }
            }
        }
    }

    Component {
        id: navboxParameter

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

                color: _color.si_text_dark
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

                onTextValueChanged: function (value) {
                    textBoxParameterWrapper.paramValueChanged(parseFloat(text));
                }
            }
        }
    }

    Component {
        id: navbox3ComponentParameter

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

                color: _color.si_text_dark
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

                onTextValueChanged: function (value) {
                    const valueX = parseFloat(paramValueX.text);
                    const valueY = parseFloat(paramValueY.text);
                    const valueZ = parseFloat(paramValueZ.text);
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

                onTextValueChanged: function (value) {
                    const valueX = parseFloat(paramValueX.text);
                    const valueY = parseFloat(paramValueY.text);
                    const valueZ = parseFloat(paramValueZ.text);
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

                onTextValueChanged: function (value) {
                    const valueX = parseFloat(paramValueX.text);
                    const valueY = parseFloat(paramValueY.text);
                    const valueZ = parseFloat(paramValueZ.text);
                    textBox3ComponentParameterWrapper.paramValueChanged(valueX, valueY, valueZ);
                }
            }
        }
    }
}
