// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: roundedBoxInfoWidgetComponent

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

        text: "ROUNDED BOX"
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
                object: roundedBoxInfoWidgetComponent.object
                Layout.fillWidth: true
            }

            Text {
                text: "Geometry"
                color: _color.si_text_dark
                font: _fonts.label_accent
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            SiParamRow {
                id: radiusRow

                Layout.fillWidth: true

                label: "Radius"
                value: roundedBoxInfoWidgetComponent.object ? roundedBoxInfoWidgetComponent.object.radius : 0.0
                enabled: roundedBoxInfoWidgetComponent.object !== null

                onCommitted: {
                    if (roundedBoxInfoWidgetComponent.object !== null)
                        roundedBoxInfoWidgetComponent.object.radius = value;
                }
            }

            SiParamRow {
                id: sizeXRow

                Layout.fillWidth: true

                label: "Size X"
                value: roundedBoxInfoWidgetComponent.object ? roundedBoxInfoWidgetComponent.object.sizeX : 0.0
                enabled: roundedBoxInfoWidgetComponent.object !== null

                onCommitted: {
                    if (roundedBoxInfoWidgetComponent.object !== null)
                        roundedBoxInfoWidgetComponent.object.sizeX = value;
                }
            }

            SiParamRow {
                id: sizeYRow

                Layout.fillWidth: true

                label: "Size Y"
                value: roundedBoxInfoWidgetComponent.object ? roundedBoxInfoWidgetComponent.object.sizeY : 0.0
                enabled: roundedBoxInfoWidgetComponent.object !== null

                onCommitted: {
                    if (roundedBoxInfoWidgetComponent.object !== null)
                        roundedBoxInfoWidgetComponent.object.sizeY = value;
                }
            }

            SiParamRow {
                id: sizeZRow

                Layout.fillWidth: true

                label: "Size Z"
                value: roundedBoxInfoWidgetComponent.object ? roundedBoxInfoWidgetComponent.object.sizeZ : 0.0
                enabled: roundedBoxInfoWidgetComponent.object !== null

                onCommitted: {
                    if (roundedBoxInfoWidgetComponent.object !== null)
                        roundedBoxInfoWidgetComponent.object.sizeZ = value;
                }
            }

            SiParamRow {
                id: slicesRow

                Layout.fillWidth: true

                label: "Slices"
                integral: true
                value: roundedBoxInfoWidgetComponent.object ? roundedBoxInfoWidgetComponent.object.slices : 0
                enabled: roundedBoxInfoWidgetComponent.object !== null

                onCommitted: {
                    if (roundedBoxInfoWidgetComponent.object !== null)
                        roundedBoxInfoWidgetComponent.object.slices = Math.max(1, value);
                }
            }

            SiParamRow {
                id: segmentsXRow

                Layout.fillWidth: true

                label: "Segments X"
                integral: true
                value: roundedBoxInfoWidgetComponent.object ? roundedBoxInfoWidgetComponent.object.segmentsX : 0
                enabled: roundedBoxInfoWidgetComponent.object !== null

                onCommitted: {
                    if (roundedBoxInfoWidgetComponent.object !== null)
                        roundedBoxInfoWidgetComponent.object.segmentsX = Math.max(1, value);
                }
            }

            SiParamRow {
                id: segmentsYRow

                Layout.fillWidth: true

                label: "Segments Y"
                integral: true
                value: roundedBoxInfoWidgetComponent.object ? roundedBoxInfoWidgetComponent.object.segmentsY : 0
                enabled: roundedBoxInfoWidgetComponent.object !== null

                onCommitted: {
                    if (roundedBoxInfoWidgetComponent.object !== null)
                        roundedBoxInfoWidgetComponent.object.segmentsY = Math.max(1, value);
                }
            }

            SiParamRow {
                id: segmentsZRow

                Layout.fillWidth: true

                label: "Segments Z"
                integral: true
                value: roundedBoxInfoWidgetComponent.object ? roundedBoxInfoWidgetComponent.object.segmentsZ : 0
                enabled: roundedBoxInfoWidgetComponent.object !== null

                onCommitted: {
                    if (roundedBoxInfoWidgetComponent.object !== null)
                        roundedBoxInfoWidgetComponent.object.segmentsZ = Math.max(1, value);
                }
            }
        }
    }

    onObjectChanged: {
        const o = roundedBoxInfoWidgetComponent.object;

        if (o === null)
            return;

        radiusRow.applyValue(o.radius);
        sizeXRow.applyValue(o.sizeX);
        sizeYRow.applyValue(o.sizeY);
        sizeZRow.applyValue(o.sizeZ);
        slicesRow.applyValue(o.slices);
        segmentsXRow.applyValue(o.segmentsX);
        segmentsYRow.applyValue(o.segmentsY);
        segmentsZRow.applyValue(o.segmentsZ);
    }
}
