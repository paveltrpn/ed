// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: gridComponent

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    color: _color.popup_background

    radius: _units.radiusEight

    implicitHeight: gridSettingsLayout.implicitHeight + _units.full

    ColumnLayout {
        id: gridSettingsLayout

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
            id: gridSizeField
            sourceComponent: gridParameter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: _units.scaled_24

            onLoaded: {
                item.parameterlabel = "Grid size";
                item.parameterValue = Tired.scenegraph.grid.gridSize;
            }

            Connections {
                target: gridSizeField.item
                function onParamValueChanged(value) {
                    Tired.scenegraph.grid.gridSize = value;
                }
            }
        }

        Loader {
            id: lineThicknessField
            sourceComponent: gridParameter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: _units.scaled_24

            onLoaded: {
                item.parameterlabel = "Line thickness";
                item.parameterValue = Tired.scenegraph.grid.lineThickness;
            }

            Connections {
                target: lineThicknessField.item
                function onParamValueChanged(value) {
                    Tired.scenegraph.grid.lineThickness = value;
                }
            }
        }

        Loader {
            id: maxRangeField
            sourceComponent: gridParameter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: _units.scaled_24

            onLoaded: {
                item.parameterlabel = "Max range";
                item.parameterValue = Tired.scenegraph.grid.maxRange;
            }

            Connections {
                target: maxRangeField.item
                function onParamValueChanged(value) {
                    Tired.scenegraph.grid.maxRange = value;
                }
            }
        }

        Loader {
            id: zoomSensitivityField
            sourceComponent: gridParameter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: _units.scaled_24

            onLoaded: {
                item.parameterlabel = "Zoom sensitivity";
                item.parameterValue = Tired.scenegraph.grid.zoomSensitivity;
            }

            Connections {
                target: zoomSensitivityField.item
                function onParamValueChanged(value) {
                    Tired.scenegraph.grid.zoomSensitivity = value;
                }
            }
        }

        Loader {
            id: majorDivisorField
            sourceComponent: gridParameter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: _units.scaled_24

            onLoaded: {
                item.parameterlabel = "Major divisor";
                item.parameterValue = Tired.scenegraph.grid.majorDivisor;
            }

            Connections {
                target: majorDivisorField.item
                function onParamValueChanged(value) {
                    Tired.scenegraph.grid.majorDivisor = value;
                }
            }
        }

        Loader {
            id: gridScaleField
            sourceComponent: gridParameter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: _units.scaled_24

            onLoaded: {
                item.parameterlabel = "Grid scale";
                item.parameterValue = Tired.scenegraph.grid.gridScale;
            }

            Connections {
                target: gridScaleField.item
                function onParamValueChanged(value) {
                    Tired.scenegraph.grid.gridScale = value;
                }
            }
        }

        Loader {
            id: gridZOffsetField
            sourceComponent: gridParameter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: _units.scaled_24

            onLoaded: {
                item.parameterlabel = "Z Offset";
                item.parameterValue = Tired.scenegraph.grid.gridZOffset;
            }

            Connections {
                target: gridZOffsetField.item
                function onParamValueChanged(value) {
                    Tired.scenegraph.grid.gridZOffset = value;
                }
            }
        }
    }

    Component {
        id: gridParameter

        Item {
            id: gridParameterWrapper

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
                    leftMargin: _units.half
                    right: paramValue.left
                }

                text: gridParameterWrapper.parameterlabel

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

                width: _units.scaled_80

                text: gridParameterWrapper.parameterValue.toFixed(3)

                onTextChanged: {
                    gridParameterWrapper.paramValueChanged(parseFloat(text));
                }
            }
        }
    }
}
