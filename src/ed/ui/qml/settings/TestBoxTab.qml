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

    bottomLeftRadius: _radius.half
    bottomRightRadius: _radius.half

    color: _color.background_overlay_60

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
            id: gridSizeField
            sourceComponent: testBoxParameter
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
            sourceComponent: testBoxParameter
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
            sourceComponent: testBoxParameter
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
    }

    Component {
        id: testBoxParameter

        Rectangle {
            id: textBoxParameterWrapper

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

                text: textBoxParameterWrapper.parameterlabel

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

                text: textBoxParameterWrapper.parameterValue.toFixed(3)

                onTextChanged: {
                    textBoxParameterWrapper.paramValueChanged(parseFloat(text));
                }
            }
        }
    }
}
