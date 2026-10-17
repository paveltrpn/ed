// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: icosphereInfoWidgetComponent

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

        text: "ICOSPHERE"
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
                object: icosphereInfoWidgetComponent.object
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
                value: icosphereInfoWidgetComponent.object ? icosphereInfoWidgetComponent.object.radius : 0.0
                enabled: icosphereInfoWidgetComponent.object !== null

                onCommitted: {
                    if (icosphereInfoWidgetComponent.object !== null)
                        icosphereInfoWidgetComponent.object.radius = value;
                }
            }

            SiParamRow {
                id: segmentsRow

                Layout.fillWidth: true

                label: "Segments"
                integral: true
                value: icosphereInfoWidgetComponent.object ? icosphereInfoWidgetComponent.object.segments : 0
                enabled: icosphereInfoWidgetComponent.object !== null

                onCommitted: {
                    if (icosphereInfoWidgetComponent.object !== null)
                        icosphereInfoWidgetComponent.object.segments = Math.max(1, value);
                }
            }
        }
    }

    onObjectChanged: {
        const o = icosphereInfoWidgetComponent.object;

        if (o === null)
            return;

        radiusRow.applyValue(o.radius);
        segmentsRow.applyValue(o.segments);
    }
}
