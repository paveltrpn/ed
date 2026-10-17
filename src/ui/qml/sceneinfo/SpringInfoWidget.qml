// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: springInfoWidgetComponent

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

        text: "SPRING"
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
                object: springInfoWidgetComponent.object
                Layout.fillWidth: true
            }

            Text {
                text: "Geometry"
                color: _color.si_text_dark
                font: _fonts.label_accent
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            SiParamRow {
                id: minorRow

                Layout.fillWidth: true

                label: "Minor"
                value: springInfoWidgetComponent.object ? springInfoWidgetComponent.object.minor : 0.0
                enabled: springInfoWidgetComponent.object !== null

                onCommitted: {
                    if (springInfoWidgetComponent.object !== null)
                        springInfoWidgetComponent.object.minor = value;
                }
            }

            SiParamRow {
                id: majorRow

                Layout.fillWidth: true

                label: "Major"
                value: springInfoWidgetComponent.object ? springInfoWidgetComponent.object.major : 0.0
                enabled: springInfoWidgetComponent.object !== null

                onCommitted: {
                    if (springInfoWidgetComponent.object !== null)
                        springInfoWidgetComponent.object.major = value;
                }
            }

            SiParamRow {
                id: sizeRow

                Layout.fillWidth: true

                label: "Size"
                value: springInfoWidgetComponent.object ? springInfoWidgetComponent.object.size : 0.0
                enabled: springInfoWidgetComponent.object !== null

                onCommitted: {
                    if (springInfoWidgetComponent.object !== null)
                        springInfoWidgetComponent.object.size = value;
                }
            }

            SiParamRow {
                id: slicesRow

                Layout.fillWidth: true

                label: "Slices"
                integral: true
                value: springInfoWidgetComponent.object ? springInfoWidgetComponent.object.slices : 0
                enabled: springInfoWidgetComponent.object !== null

                onCommitted: {
                    if (springInfoWidgetComponent.object !== null)
                        springInfoWidgetComponent.object.slices = Math.max(1, value);
                }
            }

            SiParamRow {
                id: segmentsRow

                Layout.fillWidth: true

                label: "Segments"
                integral: true
                value: springInfoWidgetComponent.object ? springInfoWidgetComponent.object.segments : 0
                enabled: springInfoWidgetComponent.object !== null

                onCommitted: {
                    if (springInfoWidgetComponent.object !== null)
                        springInfoWidgetComponent.object.segments = Math.max(1, value);
                }
            }

            SiParamRow {
                id: minorStartRow

                Layout.fillWidth: true

                label: "Minor Start"
                value: springInfoWidgetComponent.object ? springInfoWidgetComponent.object.minorStart : 0.0
                enabled: springInfoWidgetComponent.object !== null

                onCommitted: {
                    if (springInfoWidgetComponent.object !== null)
                        springInfoWidgetComponent.object.minorStart = value;
                }
            }

            SiParamRow {
                id: minorSweepRow

                Layout.fillWidth: true

                label: "Minor Sweep"
                value: springInfoWidgetComponent.object ? springInfoWidgetComponent.object.minorSweep : 0.0
                enabled: springInfoWidgetComponent.object !== null

                onCommitted: {
                    if (springInfoWidgetComponent.object !== null)
                        springInfoWidgetComponent.object.minorSweep = value;
                }
            }

            SiParamRow {
                id: majorStartRow

                Layout.fillWidth: true

                label: "Major Start"
                value: springInfoWidgetComponent.object ? springInfoWidgetComponent.object.majorStart : 0.0
                enabled: springInfoWidgetComponent.object !== null

                onCommitted: {
                    if (springInfoWidgetComponent.object !== null)
                        springInfoWidgetComponent.object.majorStart = value;
                }
            }

            SiParamRow {
                id: majorSweepRow

                Layout.fillWidth: true

                label: "Major Sweep"
                value: springInfoWidgetComponent.object ? springInfoWidgetComponent.object.majorSweep : 0.0
                enabled: springInfoWidgetComponent.object !== null

                onCommitted: {
                    if (springInfoWidgetComponent.object !== null)
                        springInfoWidgetComponent.object.majorSweep = value;
                }
            }
        }
    }

    onObjectChanged: {
        const o = springInfoWidgetComponent.object;

        if (o === null)
            return;

        minorRow.applyValue(o.minor);
        majorRow.applyValue(o.major);
        sizeRow.applyValue(o.size);
        slicesRow.applyValue(o.slices);
        segmentsRow.applyValue(o.segments);
        minorStartRow.applyValue(o.minorStart);
        minorSweepRow.applyValue(o.minorSweep);
        majorStartRow.applyValue(o.majorStart);
        majorSweepRow.applyValue(o.majorSweep);
    }
}
