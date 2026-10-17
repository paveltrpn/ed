// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: torusKnotInfoWidgetComponent

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

        text: "TORUS KNOT"
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
                object: torusKnotInfoWidgetComponent.object
                Layout.fillWidth: true
            }

            Text {
                text: "Geometry"
                color: _color.si_text_dark
                font: _fonts.label_accent
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            SiParamRow {
                id: pRow

                Layout.fillWidth: true

                label: "P"
                integral: true
                value: torusKnotInfoWidgetComponent.object ? torusKnotInfoWidgetComponent.object.p : 0
                enabled: torusKnotInfoWidgetComponent.object !== null

                onCommitted: {
                    if (torusKnotInfoWidgetComponent.object !== null)
                        torusKnotInfoWidgetComponent.object.p = Math.max(1, value);
                }
            }

            SiParamRow {
                id: qRow

                Layout.fillWidth: true

                label: "Q"
                integral: true
                value: torusKnotInfoWidgetComponent.object ? torusKnotInfoWidgetComponent.object.q : 0
                enabled: torusKnotInfoWidgetComponent.object !== null

                onCommitted: {
                    if (torusKnotInfoWidgetComponent.object !== null)
                        torusKnotInfoWidgetComponent.object.q = Math.max(1, value);
                }
            }

            SiParamRow {
                id: slicesRow

                Layout.fillWidth: true

                label: "Slices"
                integral: true
                value: torusKnotInfoWidgetComponent.object ? torusKnotInfoWidgetComponent.object.slices : 0
                enabled: torusKnotInfoWidgetComponent.object !== null

                onCommitted: {
                    if (torusKnotInfoWidgetComponent.object !== null)
                        torusKnotInfoWidgetComponent.object.slices = Math.max(1, value);
                }
            }

            SiParamRow {
                id: segmentsRow

                Layout.fillWidth: true

                label: "Segments"
                integral: true
                value: torusKnotInfoWidgetComponent.object ? torusKnotInfoWidgetComponent.object.segments : 0
                enabled: torusKnotInfoWidgetComponent.object !== null

                onCommitted: {
                    if (torusKnotInfoWidgetComponent.object !== null)
                        torusKnotInfoWidgetComponent.object.segments = Math.max(1, value);
                }
            }
        }
    }

    onObjectChanged: {
        const o = torusKnotInfoWidgetComponent.object;

        if (o === null)
            return;

        pRow.applyValue(o.p);
        qRow.applyValue(o.q);
        slicesRow.applyValue(o.slices);
        segmentsRow.applyValue(o.segments);
    }
}
