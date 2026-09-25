// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: boxInfoWidgetComponent

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    property var object: null

    anchors {
        fill: parent
    }

    color: _color.si_background_light

    Text {
        id: typeLabel

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        height: _units.scaled_32

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter

        font: _fonts.text_body_accent

        color: _color.si_text_dark

        text: "BOX"
    }

    Flickable {
        id: contentFlickable

        anchors {
            top: typeLabel.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        clip: true

        contentHeight: contentColumn.implicitHeight + _units.full

        ColumnLayout {
            id: contentColumn

            anchors {
                top: parent.top
                topMargin: _units.full
                left: parent.left
                leftMargin: _units.full
                right: parent.right
                rightMargin: _units.full
            }

            spacing: _units.half

            SiCommonParams {
                object: boxInfoWidgetComponent.object
                Layout.fillWidth: true
            }

            Text {
                text: "Segments"
                color: _color.si_text_dark
                font: _fonts.label_accent
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            SiParamRow {
                id: segmentsXRow

                Layout.fillWidth: true

                label: "Segments X"
                integral: true
                value: boxInfoWidgetComponent.object ? boxInfoWidgetComponent.object.segmentsX : 0.0
                enabled: boxInfoWidgetComponent.object !== null

                onCommitted: {
                    if (boxInfoWidgetComponent.object !== null)
                        boxInfoWidgetComponent.object.segmentsX = Math.max(1, value);
                }
            }

            SiParamRow {
                id: segmentsYRow

                Layout.fillWidth: true

                label: "Segments Y"
                integral: true
                value: boxInfoWidgetComponent.object ? boxInfoWidgetComponent.object.segmentsY : 0.0
                enabled: boxInfoWidgetComponent.object !== null

                onCommitted: {
                    if (boxInfoWidgetComponent.object !== null)
                        boxInfoWidgetComponent.object.segmentsY = Math.max(1, value);
                }
            }

            SiParamRow {
                id: segmentsZRow

                Layout.fillWidth: true

                label: "Segments Z"
                integral: true
                value: boxInfoWidgetComponent.object ? boxInfoWidgetComponent.object.segmentsZ : 0.0
                enabled: boxInfoWidgetComponent.object !== null

                onCommitted: {
                    if (boxInfoWidgetComponent.object !== null)
                        boxInfoWidgetComponent.object.segmentsZ = Math.max(1, value);
                }
            }
        }
    }

    onObjectChanged: {
        const o = boxInfoWidgetComponent.object;

        if (o === null)
            return;

        segmentsXRow.applyValue(o.segmentsX);
        segmentsYRow.applyValue(o.segmentsY);
        segmentsZRow.applyValue(o.segmentsZ);
    }
}
