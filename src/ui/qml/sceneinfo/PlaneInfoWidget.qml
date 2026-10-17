// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: planeInfoWidgetComponent

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

        text: "PLANE"
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
                object: planeInfoWidgetComponent.object
                Layout.fillWidth: true
            }

            Text {
                text: "Geometry"
                color: _color.si_text_dark
                font: _fonts.label_accent
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            SiParamRow {
                id: sizeXRow

                Layout.fillWidth: true

                label: "Size X"
                value: planeInfoWidgetComponent.object ? planeInfoWidgetComponent.object.sizeX : 0.0
                enabled: planeInfoWidgetComponent.object !== null

                onCommitted: {
                    if (planeInfoWidgetComponent.object !== null)
                        planeInfoWidgetComponent.object.sizeX = value;
                }
            }

            SiParamRow {
                id: sizeYRow

                Layout.fillWidth: true

                label: "Size Y"
                value: planeInfoWidgetComponent.object ? planeInfoWidgetComponent.object.sizeY : 0.0
                enabled: planeInfoWidgetComponent.object !== null

                onCommitted: {
                    if (planeInfoWidgetComponent.object !== null)
                        planeInfoWidgetComponent.object.sizeY = value;
                }
            }

            SiParamRow {
                id: segmentsXRow

                Layout.fillWidth: true

                label: "Segments X"
                integral: true
                value: planeInfoWidgetComponent.object ? planeInfoWidgetComponent.object.segmentsX : 0
                enabled: planeInfoWidgetComponent.object !== null

                onCommitted: {
                    if (planeInfoWidgetComponent.object !== null)
                        planeInfoWidgetComponent.object.segmentsX = Math.max(1, value);
                }
            }

            SiParamRow {
                id: segmentsYRow

                Layout.fillWidth: true

                label: "Segments Y"
                integral: true
                value: planeInfoWidgetComponent.object ? planeInfoWidgetComponent.object.segmentsY : 0
                enabled: planeInfoWidgetComponent.object !== null

                onCommitted: {
                    if (planeInfoWidgetComponent.object !== null)
                        planeInfoWidgetComponent.object.segmentsY = Math.max(1, value);
                }
            }
        }
    }

    onObjectChanged: {
        const o = planeInfoWidgetComponent.object;

        if (o === null)
            return;

        sizeXRow.applyValue(o.sizeX);
        sizeYRow.applyValue(o.sizeY);
        segmentsXRow.applyValue(o.segmentsX);
        segmentsYRow.applyValue(o.segmentsY);
    }
}
