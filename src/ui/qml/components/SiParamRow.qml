// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0

Item {
    id: siParamRowComponent

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    property string label: ""
    property real value: 0.0
    property bool integral: false
    property bool enabled: true

    signal committed(real value)

    implicitHeight: _units.scaled_28

    Text {
        id: paramLabel

        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            leftMargin: _units.half
            right: paramValue.left
        }

        text: siParamRowComponent.label

        verticalAlignment: Text.AlignVCenter

        color: _color.si_text_dark
        font: _fonts.label

        elide: Text.ElideRight
    }

    TiredTextInput {
        id: paramValue

        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
            rightMargin: _units.scaled_8
        }

        width: _units.scaled_80

        enabled: siParamRowComponent.enabled

        text: siParamRowComponent.formatValue(siParamRowComponent.value)

        onTextValueChanged: function (text) {
            const parsed = siParamRowComponent.integral ? parseInt(text) : parseFloat(text);

            if (isNaN(parsed)) {
                paramValue.text = siParamRowComponent.formatValue(siParamRowComponent.value);
                return;
            }

            siParamRowComponent.committed(parsed);
        }
    }

    function formatValue(v) {
        return siParamRowComponent.integral ? Math.round(v).toString() : v.toFixed(3);
    }

    function applyValue(v) {
        siParamRowComponent.value = v;
        paramValue.text = siParamRowComponent.formatValue(v);
    }
}
