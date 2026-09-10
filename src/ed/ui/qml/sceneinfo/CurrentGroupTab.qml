// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: currentGroupComponent

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    bottomLeftRadius: _units.radiusEight
    bottomRightRadius: _units.radiusEight

    color: _color.si_background_light

    implicitHeight: currentGroupInfoLayout.implicitHeight + _units.full

    ColumnLayout {
        id: currentGroupInfoLayout

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

        Item {
            id: numericParameterWrapper

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
                }

                text: numericParameterWrapper.parameterlabel

                verticalAlignment: Text.AlignVCenter

                color: _color.si_text_dark
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
