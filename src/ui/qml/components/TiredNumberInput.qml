import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0

TextInput {
    id: numberInputComponent

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    property bool integral: false

    height: _units.scaled_24
    color: _color.si_text_dark
    font: _fonts.label
    verticalAlignment: Text.AlignVCenter

    leftPadding: _units.full

    clip: true

    validator: RegularExpressionValidator {
        regularExpression: {
            if (numberInputComponent.integral) {
                return /^-?\d*$/;
            }
            return /^-?\d*\.?\d*$/;
        }
    }

    signal textValueChanged(string value)

    function commitText() {
        let raw = numberInputComponent.text;
        let parsed = Number(raw);
        if (raw.trim() === "" || Number.isNaN(parsed)) {
            parsed = 0;
        }
        if (numberInputComponent.integral) {
            numberInputComponent.text = Math.round(parsed).toString();
        } else {
            numberInputComponent.text = parsed.toFixed(3);
        }
        numberInputComponent.textValueChanged(String(parsed));
    }

    onAccepted: {
        numberInputComponent.commitText();
    }

    onActiveFocusChanged: {
        if (!activeFocus) {
            numberInputComponent.commitText();
        }
    }

    Rectangle {
        z: -1
        anchors.fill: parent
        color: {
            if (inputArea.containsMouse) {
                return _color.background_overlay_light_40;
            }
            return _color.si_background_light;
        }

        border {
            color: _color.si_background_dark;
            width: _units.scaled_1;
        }

        radius: _units.radiusEight

        MouseArea {
            id: inputArea
            hoverEnabled: true
            anchors.fill: parent
        }
    }
}
