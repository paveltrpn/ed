import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: sceneTotalInfoComponent

    readonly property var _fonts: Appearence.fonts
    readonly property var _gaps: Appearence.gaps
    readonly property var _radius: Appearence.radius
    readonly property var _color: Appearence.colors

    bottomLeftRadius: _radius.half
    bottomRightRadius: _radius.half

    color: _color.background_overlay_60

    implicitHeight: sceneTotalInfoLayout.implicitHeight + _gaps.full

    ColumnLayout {
        id: sceneTotalInfoLayout

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
            id: dummyField
            sourceComponent: numericParameter
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 24

            onLoaded: {
                item.parameterlabel = "some dummy param";
                item.parameterValue = 0.0;
            }

            Connections {
            }
        }
    }

    Component {
        id: numericParameter

        Rectangle {
            id: numericParameterWrapper

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

                text: numericParameterWrapper.parameterlabel

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

                text: numericParameterWrapper.parameterValue.toFixed(3)

                onTextChanged: {
                    numericParameterWrapper.paramValueChanged(parseFloat(text));
                }
            }
        }
    }
}
