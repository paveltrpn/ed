// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: cappedTubeInfoWidgetComponent

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

        text: "CAPPED TUBE"
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
                object: cappedTubeInfoWidgetComponent.object
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
                value: cappedTubeInfoWidgetComponent.object ? cappedTubeInfoWidgetComponent.object.radius : 0.0
                enabled: cappedTubeInfoWidgetComponent.object !== null

                onCommitted: {
                    if (cappedTubeInfoWidgetComponent.object !== null)
                        cappedTubeInfoWidgetComponent.object.radius = value;
                }
            }

            SiParamRow {
                id: innerRadiusRow

                Layout.fillWidth: true

                label: "Inner Radius"
                value: cappedTubeInfoWidgetComponent.object ? cappedTubeInfoWidgetComponent.object.innerRadius : 0.0
                enabled: cappedTubeInfoWidgetComponent.object !== null

                onCommitted: {
                    if (cappedTubeInfoWidgetComponent.object !== null)
                        cappedTubeInfoWidgetComponent.object.innerRadius = value;
                }
            }

            SiParamRow {
                id: sizeRow

                Layout.fillWidth: true

                label: "Size"
                value: cappedTubeInfoWidgetComponent.object ? cappedTubeInfoWidgetComponent.object.size : 0.0
                enabled: cappedTubeInfoWidgetComponent.object !== null

                onCommitted: {
                    if (cappedTubeInfoWidgetComponent.object !== null)
                        cappedTubeInfoWidgetComponent.object.size = value;
                }
            }

            SiParamRow {
                id: slicesRow

                Layout.fillWidth: true

                label: "Slices"
                integral: true
                value: cappedTubeInfoWidgetComponent.object ? cappedTubeInfoWidgetComponent.object.slices : 0
                enabled: cappedTubeInfoWidgetComponent.object !== null

                onCommitted: {
                    if (cappedTubeInfoWidgetComponent.object !== null)
                        cappedTubeInfoWidgetComponent.object.slices = Math.max(1, value);
                }
            }

            SiParamRow {
                id: segmentsRow

                Layout.fillWidth: true

                label: "Segments"
                integral: true
                value: cappedTubeInfoWidgetComponent.object ? cappedTubeInfoWidgetComponent.object.segments : 0
                enabled: cappedTubeInfoWidgetComponent.object !== null

                onCommitted: {
                    if (cappedTubeInfoWidgetComponent.object !== null)
                        cappedTubeInfoWidgetComponent.object.segments = Math.max(1, value);
                }
            }

            SiParamRow {
                id: ringsRow

                Layout.fillWidth: true

                label: "Rings"
                integral: true
                value: cappedTubeInfoWidgetComponent.object ? cappedTubeInfoWidgetComponent.object.rings : 0
                enabled: cappedTubeInfoWidgetComponent.object !== null

                onCommitted: {
                    if (cappedTubeInfoWidgetComponent.object !== null)
                        cappedTubeInfoWidgetComponent.object.rings = Math.max(1, value);
                }
            }

            SiParamRow {
                id: startRow

                Layout.fillWidth: true

                label: "Start"
                value: cappedTubeInfoWidgetComponent.object ? cappedTubeInfoWidgetComponent.object.start : 0.0
                enabled: cappedTubeInfoWidgetComponent.object !== null

                onCommitted: {
                    if (cappedTubeInfoWidgetComponent.object !== null)
                        cappedTubeInfoWidgetComponent.object.start = value;
                }
            }

            SiParamRow {
                id: sweepRow

                Layout.fillWidth: true

                label: "Sweep"
                value: cappedTubeInfoWidgetComponent.object ? cappedTubeInfoWidgetComponent.object.sweep : 0.0
                enabled: cappedTubeInfoWidgetComponent.object !== null

                onCommitted: {
                    if (cappedTubeInfoWidgetComponent.object !== null)
                        cappedTubeInfoWidgetComponent.object.sweep = value;
                }
            }
        }
    }

    onObjectChanged: {
        const o = cappedTubeInfoWidgetComponent.object;

        if (o === null)
            return;

        radiusRow.applyValue(o.radius);
        innerRadiusRow.applyValue(o.innerRadius);
        sizeRow.applyValue(o.size);
        slicesRow.applyValue(o.slices);
        segmentsRow.applyValue(o.segments);
        ringsRow.applyValue(o.rings);
        startRow.applyValue(o.start);
        sweepRow.applyValue(o.sweep);
    }
}
