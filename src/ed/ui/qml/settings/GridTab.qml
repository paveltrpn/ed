// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: gridComponent

    readonly property var _fonts: Appearence.fonts
    readonly property var _gaps: Appearence.gaps
    readonly property var _radius: Appearence.radius
    readonly property var _color: Appearence.colors

    bottomLeftRadius: _radius.half
    bottomRightRadius: _radius.half

    color: _color.background_overlay_60

    implicitHeight: gridSettingsLayout.implicitHeight + _gaps.full

    ColumnLayout {
        id: gridSettingsLayout

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
            id: gridSizeField
            sourceComponent: gridParameter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 24

            onLoaded: {
                item.parameterlabel = "gridSize";
                item.parameterValue = Tired.grid.gridSize;
            }

            Connections {
                target: gridSizeField.item
                function onParamValueChanged(value) {
                    Tired.grid.gridSize = value;
                }
            }
        }

        Loader {
            id: lineThicknessField
            sourceComponent: gridParameter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 24

            onLoaded: {
                item.parameterlabel = "lineThickness";
                item.parameterValue = Tired.grid.lineThickness;
            }

            Connections {
                target: lineThicknessField.item
                function onParamValueChanged(value) {
                    Tired.grid.lineThickness = value;
                }
            }
        }

        Loader {
            id: maxRangeField
            sourceComponent: gridParameter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 24

            onLoaded: {
                item.parameterlabel = "maxRange";
                item.parameterValue = Tired.grid.maxRange;
            }

            Connections {
                target: maxRangeField.item
                function onParamValueChanged(value) {
                    Tired.grid.maxRange = value;
                }
            }
        }

        Loader {
            id: zoomSensitivityField
            sourceComponent: gridParameter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 24

            onLoaded: {
                item.parameterlabel = "zoomSensitivity";
                item.parameterValue = Tired.grid.zoomSensitivity;
            }

            Connections {
                target: zoomSensitivityField.item
                function onParamValueChanged(value) {
                    Tired.grid.zoomSensitivity = value;
                }
            }
        }

        Loader {
            id: majorDivisorField
            sourceComponent: gridParameter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 24

            onLoaded: {
                item.parameterlabel = "majorDivisor";
                item.parameterValue = Tired.grid.majorDivisor;
            }

            Connections {
                target: majorDivisorField.item
                function onParamValueChanged(value) {
                    Tired.grid.majorDivisor = value;
                }
            }
        }

        Loader {
            id: gridScaleField
            sourceComponent: gridParameter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 24

            onLoaded: {
                item.parameterlabel = "gridScale";
                item.parameterValue = Tired.grid.gridScale;
            }

            Connections {
                target: gridScaleField.item
                function onParamValueChanged(value) {
                    Tired.grid.gridScale = value;
                }
            }
        }

        Loader {
            id: gridZOffsetField
            sourceComponent: gridParameter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 24

            onLoaded: {
                item.parameterlabel = "gridZOffset";
                item.parameterValue = Tired.grid.gridZOffset;
            }

            Connections {
                target: gridZOffsetField.item
                function onParamValueChanged(value) {
                    Tired.grid.gridZOffset = value;
                }
            }
        }
    }

    Component {
        id: gridParameter

        Rectangle {
            id: gridParameterWrapper

            anchors.fill: parent
            color: _color.background_overlay_60

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
                }

                text: gridParameterWrapper.parameterlabel

                verticalAlignment: Text.AlignVCenter

                color: _color.main_contrast
                font: _fonts.label
            }

            TiredTextInput {
                id: paramValue

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    right: parent.right
                    rightMargin: 8
                }

                width: 92

                text: gridParameterWrapper.parameterValue.toFixed(3)

                onTextChanged: {
                    gridParameterWrapper.paramValueChanged(parseFloat(text));
                }
            }
        }
    }
}
