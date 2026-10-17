// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: diskInfoWidgetComponent

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

        text: "DISK"
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
                object: diskInfoWidgetComponent.object
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
                value: diskInfoWidgetComponent.object ? diskInfoWidgetComponent.object.radius : 0.0
                enabled: diskInfoWidgetComponent.object !== null

                onCommitted: {
                    if (diskInfoWidgetComponent.object !== null)
                        diskInfoWidgetComponent.object.radius = value;
                }
            }

            SiParamRow {
                id: innerRadiusRow

                Layout.fillWidth: true

                label: "Inner Radius"
                value: diskInfoWidgetComponent.object ? diskInfoWidgetComponent.object.innerRadius : 0.0
                enabled: diskInfoWidgetComponent.object !== null

                onCommitted: {
                    if (diskInfoWidgetComponent.object !== null)
                        diskInfoWidgetComponent.object.innerRadius = value;
                }
            }

            SiParamRow {
                id: slicesRow

                Layout.fillWidth: true

                label: "Slices"
                integral: true
                value: diskInfoWidgetComponent.object ? diskInfoWidgetComponent.object.slices : 0
                enabled: diskInfoWidgetComponent.object !== null

                onCommitted: {
                    if (diskInfoWidgetComponent.object !== null)
                        diskInfoWidgetComponent.object.slices = Math.max(1, value);
                }
            }

            SiParamRow {
                id: ringsRow

                Layout.fillWidth: true

                label: "Rings"
                integral: true
                value: diskInfoWidgetComponent.object ? diskInfoWidgetComponent.object.rings : 0
                enabled: diskInfoWidgetComponent.object !== null

                onCommitted: {
                    if (diskInfoWidgetComponent.object !== null)
                        diskInfoWidgetComponent.object.rings = Math.max(1, value);
                }
            }

            SiParamRow {
                id: startRow

                Layout.fillWidth: true

                label: "Start"
                value: diskInfoWidgetComponent.object ? diskInfoWidgetComponent.object.start : 0.0
                enabled: diskInfoWidgetComponent.object !== null

                onCommitted: {
                    if (diskInfoWidgetComponent.object !== null)
                        diskInfoWidgetComponent.object.start = value;
                }
            }

            SiParamRow {
                id: sweepRow

                Layout.fillWidth: true

                label: "Sweep"
                value: diskInfoWidgetComponent.object ? diskInfoWidgetComponent.object.sweep : 0.0
                enabled: diskInfoWidgetComponent.object !== null

                onCommitted: {
                    if (diskInfoWidgetComponent.object !== null)
                        diskInfoWidgetComponent.object.sweep = value;
                }
            }
        }
    }

    onObjectChanged: {
        const o = diskInfoWidgetComponent.object;

        if (o === null)
            return;

        radiusRow.applyValue(o.radius);
        innerRadiusRow.applyValue(o.innerRadius);
        slicesRow.applyValue(o.slices);
        ringsRow.applyValue(o.rings);
        startRow.applyValue(o.start);
        sweepRow.applyValue(o.sweep);
    }
}
