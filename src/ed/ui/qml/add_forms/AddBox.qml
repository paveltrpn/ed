// qmllint disable unqualified

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Tire 1.0
import "../components"

Rectangle {
    id: addBoxCmponent

    readonly property var _fonts: Appearence.fonts
    readonly property var _gaps: Appearence.gaps
    readonly property var _radius: Appearence.radius
    readonly property var _color: Appearence.colors

    height: 216

    z: parent.z

    radius: _radius.half
    color: _color.background_overlay_60

    Text {
        id: posLabel
        anchors {
            top: parent.top
            topMargin: _gaps.full
            left: parent.left
            leftMargin: _gaps.half
            right: parent.right
            rightMargin: _gaps.half
        }

        height: 20

        text: "Position"
        color: _color.main_contrast
        font: _fonts.label
    }

    RowLayout {
        id: posInputRow
        anchors {
            top: posLabel.bottom
            topMargin: _gaps.half
            left: parent.left
            leftMargin: _gaps.half
            right: parent.right
            rightMargin: _gaps.half
        }

        height: 24

        TiredTextInput {
            id: posXInput
            Layout.preferredWidth: posInputRow.width / 3.3
            Layout.preferredHeight: parent.height
            text: "0.0"
        }
        TiredTextInput {
            id: posYInput
            Layout.preferredWidth: posInputRow.width / 3.3
            Layout.preferredHeight: parent.height
            text: "0.0"
        }
        TiredTextInput {
            id: posZInput
            Layout.preferredWidth: posInputRow.width / 3.3
            Layout.preferredHeight: parent.height
            text: "0.0"
        }
    }

    Text {
        id: rotLabel
        anchors {
            top: posInputRow.bottom
            topMargin: _gaps.full
            left: parent.left
            leftMargin: _gaps.half
            right: parent.right
            rightMargin: _gaps.half
        }

        height: 20

        text: "Rotation"
        color: _color.main_contrast
        font: _fonts.label
    }

    RowLayout {
        id: rotInputRow
        anchors {
            top: rotLabel.bottom
            topMargin: _gaps.half
            left: parent.left
            leftMargin: _gaps.half
            right: parent.right
            rightMargin: _gaps.half
        }
        height: 24

        TiredTextInput {
            id: rotXInput
            Layout.preferredWidth: posInputRow.width / 3.3
            Layout.preferredHeight: parent.height
            text: "0.0"
        }
        TiredTextInput {
            id: rotYInput
            Layout.preferredWidth: posInputRow.width / 3.3
            Layout.preferredHeight: parent.height
            text: "0.0"
        }
        TiredTextInput {
            id: rotZInput
            Layout.preferredWidth: posInputRow.width / 3.3
            Layout.preferredHeight: parent.height
            text: "0.0"
        }
    }

    Text {
        id: sclLabel
        anchors {
            top: rotInputRow.bottom
            topMargin: _gaps.full
            left: parent.left
            leftMargin: _gaps.half
            right: parent.right
            rightMargin: _gaps.half
        }

        height: 20

        text: "Scale"
        color: _color.main_contrast
        font: _fonts.label
    }

    RowLayout {
        id: sclInputRow
        anchors {
            top: sclLabel.bottom
            topMargin: _gaps.half
            left: parent.left
            leftMargin: _gaps.half
            right: parent.right
            rightMargin: _gaps.half
        }
        height: 24

        TiredTextInput {
            id: sclXInput
            Layout.preferredWidth: posInputRow.width / 3.3
            Layout.preferredHeight: parent.height
            text: "1.0"
        }
        TiredTextInput {
            id: sclYInput
            Layout.preferredWidth: posInputRow.width / 3.3
            Layout.preferredHeight: parent.height
            text: "1.0"
        }
        TiredTextInput {
            id: sclZInput
            Layout.preferredWidth: posInputRow.width / 3.3
            Layout.preferredHeight: parent.height
            text: "1.0"
        }
    }

    NpButton {
        id: saveBoxButton
        anchors {
            bottom: parent.bottom
            bottomMargin: _gaps.full
            horizontalCenter: parent.horizontalCenter
        }
        width: 128
        text: "Save"
        font: _fonts.label_accent

        onClicked: {
            const px = parseFloat(posXInput.text)
            const py = parseFloat(posYInput.text)
            const pz = parseFloat(posZInput.text)
            const rx = parseFloat(rotXInput.text)
            const ry = parseFloat(rotYInput.text)
            const rz = parseFloat(rotZInput.text)
            const sx = parseFloat(sclXInput.text)
            const sy = parseFloat(sclYInput.text)
            const sz = parseFloat(sclZInput.text)

            Tired.scenegraph.addExBox(px, py, pz, rx, ry, rz, sx, sy, sz)

            resetAll()
            leftPanelMainComponent.resetAllButtons()
        }
    }

    function resetAll() {
        posXInput.text = "0.0"
        posYInput.text = "0.0"
        posZInput.text = "0.0"

        rotXInput.text = "0.0"
        rotYInput.text = "0.0"
        rotZInput.text = "0.0"

        sclXInput.text = "1.0"
        sclYInput.text = "1.0"
        sclZInput.text = "1.0"
    }
}
