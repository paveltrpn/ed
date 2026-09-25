// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0

Item {
    id: siCommonParamsComponent

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    property var object: null

    implicitHeight: commonParamsColumn.implicitHeight

    onObjectChanged: {
        siCommonParamsComponent.syncTexts();
    }

    ColumnLayout {
        id: commonParamsColumn

        anchors.fill: parent

        spacing: _units.half

        RowLayout {
            id: nameRow

            Layout.fillWidth: true
            Layout.preferredHeight: _units.scaled_28
            spacing: _units.scaled_8

            Text {
                text: "Name"
                color: _color.si_text_dark
                font: _fonts.label
                Layout.alignment: Qt.AlignVCenter
            }

            TiredTextInput {
                id: nameInput

                Layout.fillWidth: true
                Layout.preferredHeight: _units.scaled_24

                enabled: siCommonParamsComponent.object !== null

                text: siCommonParamsComponent.object ? siCommonParamsComponent.object.name : ""

                onTextValueChanged: {
                    if (siCommonParamsComponent.object !== null) {
                        siCommonParamsComponent.object.name = nameInput.text;
                    }
                }
            }
        }

        RowLayout {
            id: positionRow

            Layout.fillWidth: true
            Layout.preferredHeight: _units.scaled_28
            spacing: _units.scaled_8

            Text {
                text: "Position"
                color: _color.si_text_dark
                font: _fonts.label
                Layout.alignment: Qt.AlignVCenter
            }

            TiredTextInput {
                id: posXInput

                Layout.fillWidth: true
                Layout.preferredHeight: _units.scaled_24

                enabled: siCommonParamsComponent.object !== null

                text: siCommonParamsComponent.object ? siCommonParamsComponent.object.position.x.toFixed(3) : "0.000"

                onTextValueChanged: siCommonParamsComponent.onVectorInputChanged("position", 0, posXInput)
            }

            TiredTextInput {
                id: posYInput

                Layout.fillWidth: true
                Layout.preferredHeight: _units.scaled_24

                enabled: siCommonParamsComponent.object !== null

                text: siCommonParamsComponent.object ? siCommonParamsComponent.object.position.y.toFixed(3) : "0.000"

                onTextValueChanged: siCommonParamsComponent.onVectorInputChanged("position", 1, posYInput)
            }

            TiredTextInput {
                id: posZInput

                Layout.fillWidth: true
                Layout.preferredHeight: _units.scaled_24

                enabled: siCommonParamsComponent.object !== null

                text: siCommonParamsComponent.object ? siCommonParamsComponent.object.position.z.toFixed(3) : "0.000"

                onTextValueChanged: siCommonParamsComponent.onVectorInputChanged("position", 2, posZInput)
            }
        }

        RowLayout {
            id: orientationRow

            Layout.fillWidth: true
            Layout.preferredHeight: _units.scaled_28
            spacing: _units.scaled_8

            Text {
                text: "Orientation"
                color: _color.si_text_dark
                font: _fonts.label
                Layout.alignment: Qt.AlignVCenter
            }

            TiredTextInput {
                id: rotXInput

                Layout.fillWidth: true
                Layout.preferredHeight: _units.scaled_24

                enabled: siCommonParamsComponent.object !== null

                text: siCommonParamsComponent.object ? siCommonParamsComponent.object.orientation.x.toFixed(3) : "0.000"

                onTextValueChanged: siCommonParamsComponent.onVectorInputChanged("orientation", 0, rotXInput)
            }

            TiredTextInput {
                id: rotYInput

                Layout.fillWidth: true
                Layout.preferredHeight: _units.scaled_24

                enabled: siCommonParamsComponent.object !== null

                text: siCommonParamsComponent.object ? siCommonParamsComponent.object.orientation.y.toFixed(3) : "0.000"

                onTextValueChanged: siCommonParamsComponent.onVectorInputChanged("orientation", 1, rotYInput)
            }

            TiredTextInput {
                id: rotZInput

                Layout.fillWidth: true
                Layout.preferredHeight: _units.scaled_24

                enabled: siCommonParamsComponent.object !== null

                text: siCommonParamsComponent.object ? siCommonParamsComponent.object.orientation.z.toFixed(3) : "0.000"

                onTextValueChanged: siCommonParamsComponent.onVectorInputChanged("orientation", 2, rotZInput)
            }
        }

        RowLayout {
            id: scaleRow

            Layout.fillWidth: true
            Layout.preferredHeight: _units.scaled_28
            spacing: _units.scaled_8

            Text {
                text: "Scale"
                color: _color.si_text_dark
                font: _fonts.label
                Layout.alignment: Qt.AlignVCenter
            }

            TiredTextInput {
                id: sclXInput

                Layout.fillWidth: true
                Layout.preferredHeight: _units.scaled_24

                enabled: siCommonParamsComponent.object !== null

                text: siCommonParamsComponent.object ? siCommonParamsComponent.object.scale.x.toFixed(3) : "1.000"

                onTextValueChanged: siCommonParamsComponent.onVectorInputChanged("scale", 0, sclXInput)
            }

            TiredTextInput {
                id: sclYInput

                Layout.fillWidth: true
                Layout.preferredHeight: _units.scaled_24

                enabled: siCommonParamsComponent.object !== null

                text: siCommonParamsComponent.object ? siCommonParamsComponent.object.scale.y.toFixed(3) : "1.000"

                onTextValueChanged: siCommonParamsComponent.onVectorInputChanged("scale", 1, sclYInput)
            }

            TiredTextInput {
                id: sclZInput

                Layout.fillWidth: true
                Layout.preferredHeight: _units.scaled_24

                enabled: siCommonParamsComponent.object !== null

                text: siCommonParamsComponent.object ? siCommonParamsComponent.object.scale.z.toFixed(3) : "1.000"

                onTextValueChanged: siCommonParamsComponent.onVectorInputChanged("scale", 2, sclZInput)
            }
        }

        RowLayout {
            id: colorRow

            Layout.fillWidth: true
            Layout.preferredHeight: _units.scaled_28
            spacing: _units.scaled_8

            Text {
                text: "Color"
                color: _color.si_text_dark
                font: _fonts.label
                Layout.alignment: Qt.AlignVCenter
            }

            TiredTextInput {
                id: colorRInput

                Layout.fillWidth: true
                Layout.preferredHeight: _units.scaled_24

                enabled: siCommonParamsComponent.object !== null

                text: siCommonParamsComponent.object ? siCommonParamsComponent.object.color.x.toFixed(3) : "0.000"

                onTextValueChanged: siCommonParamsComponent.onVectorInputChanged("color", 0, colorRInput)
            }

            TiredTextInput {
                id: colorGInput

                Layout.fillWidth: true
                Layout.preferredHeight: _units.scaled_24

                enabled: siCommonParamsComponent.object !== null

                text: siCommonParamsComponent.object ? siCommonParamsComponent.object.color.y.toFixed(3) : "0.000"

                onTextValueChanged: siCommonParamsComponent.onVectorInputChanged("color", 1, colorGInput)
            }

            TiredTextInput {
                id: colorBInput

                Layout.fillWidth: true
                Layout.preferredHeight: _units.scaled_24

                enabled: siCommonParamsComponent.object !== null

                text: siCommonParamsComponent.object ? siCommonParamsComponent.object.color.z.toFixed(3) : "0.000"

                onTextValueChanged: siCommonParamsComponent.onVectorInputChanged("color", 2, colorBInput)
            }
        }
    }

    Connections {
        target: siCommonParamsComponent.object

        function onNameChanged() {
            siCommonParamsComponent.syncTexts();
        }

        function onPositionChanged() {
            siCommonParamsComponent.syncTexts();
        }

        function onOrientationChanged() {
            siCommonParamsComponent.syncTexts();
        }

        function onScaleChanged() {
            siCommonParamsComponent.syncTexts();
        }

        function onColorChanged() {
            siCommonParamsComponent.syncTexts();
        }
    }

    function syncTexts() {
        const o = siCommonParamsComponent.object;

        if (o === null)
            return;

        if (!nameInput.activeFocus)
            nameInput.text = o.name;

        if (!posXInput.activeFocus)
            posXInput.text = o.position.x.toFixed(3);
        if (!posYInput.activeFocus)
            posYInput.text = o.position.y.toFixed(3);
        if (!posZInput.activeFocus)
            posZInput.text = o.position.z.toFixed(3);

        if (!rotXInput.activeFocus)
            rotXInput.text = o.orientation.x.toFixed(3);
        if (!rotYInput.activeFocus)
            rotYInput.text = o.orientation.y.toFixed(3);
        if (!rotZInput.activeFocus)
            rotZInput.text = o.orientation.z.toFixed(3);

        if (!sclXInput.activeFocus)
            sclXInput.text = o.scale.x.toFixed(3);
        if (!sclYInput.activeFocus)
            sclYInput.text = o.scale.y.toFixed(3);
        if (!sclZInput.activeFocus)
            sclZInput.text = o.scale.z.toFixed(3);

        if (!colorRInput.activeFocus)
            colorRInput.text = o.color.x.toFixed(3);
        if (!colorGInput.activeFocus)
            colorGInput.text = o.color.y.toFixed(3);
        if (!colorBInput.activeFocus)
            colorBInput.text = o.color.z.toFixed(3);
    }

    function onVectorInputChanged(kind, axis, input) {
        const o = siCommonParamsComponent.object;

        if (o === null)
            return;

        const parsed = parseFloat(input.text);

        if (isNaN(parsed)) {
            input.text = formattedComponent(kind, axis);
            return;
        }

        if (kind === "position")
            o.position = Qt.vector3d(axis === 0 ? parsed : o.position.x, axis === 1 ? parsed : o.position.y,
                                      axis === 2 ? parsed : o.position.z);
        else if (kind === "orientation")
            o.orientation = Qt.vector3d(axis === 0 ? parsed : o.orientation.x, axis === 1 ? parsed : o.orientation.y,
                                        axis === 2 ? parsed : o.orientation.z);
        else if (kind === "scale") {
            const clamped = Math.max(0.001, parsed);
            o.scale = Qt.vector3d(axis === 0 ? clamped : o.scale.x, axis === 1 ? clamped : o.scale.y,
                                  axis === 2 ? clamped : o.scale.z);
        } else if (kind === "color") {
            o.color = Qt.vector4d(axis === 0 ? parsed : o.color.r, axis === 1 ? parsed : o.color.g,
                                  axis === 2 ? parsed : o.color.b, axis === 3 ? parsed : o.color.a);
        }
    }

    function formattedComponent(kind, axis) {
        const o = siCommonParamsComponent.object;

        if (o === null)
            return "";

        if (kind === "position")
            return [ o.position.x, o.position.y, o.position.z ][ axis ].toFixed(3);
        if (kind === "orientation")
            return [ o.orientation.x, o.orientation.y, o.orientation.z ][ axis ].toFixed(3);
        if (kind === "scale")
            return [ o.scale.x, o.scale.y, o.scale.z ][ axis ].toFixed(3);
        return [ o.color.r, o.color.g, o.color.b, o.color.a ][ axis ].toFixed(3);
    }
}
