// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: sphereInfoWidgetComponent

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

        text: "SPHERE"
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
                object: sphereInfoWidgetComponent.object
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
                value: sphereInfoWidgetComponent.object ? sphereInfoWidgetComponent.object.radius : 0.0
                enabled: sphereInfoWidgetComponent.object !== null

                onCommitted: {
                    if (sphereInfoWidgetComponent.object !== null)
                        sphereInfoWidgetComponent.object.radius = value;
                }
            }

            SiParamRow {
                id: slicesRow

                Layout.fillWidth: true

                label: "Slices"
                integral: true
                value: sphereInfoWidgetComponent.object ? sphereInfoWidgetComponent.object.slices : 0
                enabled: sphereInfoWidgetComponent.object !== null

                onCommitted: {
                    if (sphereInfoWidgetComponent.object !== null)
                        sphereInfoWidgetComponent.object.slices = Math.max(1, value);
                }
            }

            SiParamRow {
                id: segmentsRow

                Layout.fillWidth: true

                label: "Segments"
                integral: true
                value: sphereInfoWidgetComponent.object ? sphereInfoWidgetComponent.object.segments : 0
                enabled: sphereInfoWidgetComponent.object !== null

                onCommitted: {
                    if (sphereInfoWidgetComponent.object !== null)
                        sphereInfoWidgetComponent.object.segments = Math.max(1, value);
                }
            }

            SiParamRow {
                id: sliceStartRow

                Layout.fillWidth: true

                label: "Slice start"
                value: sphereInfoWidgetComponent.object ? sphereInfoWidgetComponent.object.sliceStart : 0.0
                enabled: sphereInfoWidgetComponent.object !== null

                onCommitted: {
                    if (sphereInfoWidgetComponent.object !== null)
                        sphereInfoWidgetComponent.object.sliceStart = value;
                }
            }

            SiParamRow {
                id: sliceSweepRow

                Layout.fillWidth: true

                label: "Slice sweep"
                value: sphereInfoWidgetComponent.object ? sphereInfoWidgetComponent.object.sliceSweep : 0.0
                enabled: sphereInfoWidgetComponent.object !== null

                onCommitted: {
                    if (sphereInfoWidgetComponent.object !== null)
                        sphereInfoWidgetComponent.object.sliceSweep = value;
                }
            }

            SiParamRow {
                id: segmentStartRow

                Layout.fillWidth: true

                label: "Segment start"
                value: sphereInfoWidgetComponent.object ? sphereInfoWidgetComponent.object.segmentStart : 0.0
                enabled: sphereInfoWidgetComponent.object !== null

                onCommitted: {
                    if (sphereInfoWidgetComponent.object !== null)
                        sphereInfoWidgetComponent.object.segmentStart = value;
                }
            }

            SiParamRow {
                id: segmentSweepRow

                Layout.fillWidth: true

                label: "Segment sweep"
                value: sphereInfoWidgetComponent.object ? sphereInfoWidgetComponent.object.segmentSweep : 0.0
                enabled: sphereInfoWidgetComponent.object !== null

                onCommitted: {
                    if (sphereInfoWidgetComponent.object !== null)
                        sphereInfoWidgetComponent.object.segmentSweep = value;
                }
            }
        }
    }

    onObjectChanged: {
        const o = sphereInfoWidgetComponent.object;

        if (o === null)
            return;

        radiusRow.applyValue(o.radius);
        slicesRow.applyValue(o.slices);
        segmentsRow.applyValue(o.segments);
        sliceStartRow.applyValue(o.sliceStart);
        sliceSweepRow.applyValue(o.sliceSweep);
        segmentStartRow.applyValue(o.segmentStart);
        segmentSweepRow.applyValue(o.segmentSweep);
    }
}
