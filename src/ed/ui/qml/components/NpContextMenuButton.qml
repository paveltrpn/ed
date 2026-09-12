// qmllint disable unqualified
// qmllint disable Quick.property-changes-parsed

import QtQuick
import QtQuick.Templates as QuickTemplates
import QtQuick.Controls.impl

import Tire 1.0

QuickTemplates.Button {
    id: control

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    property bool isBorderEnabled: false
    property color backgroundColor: _color.si_background_light

    property alias topLeftRadius: backgroundItem.topLeftRadius
    property alias topRightRadius: backgroundItem.topRightRadius
    property alias bottomLeftRadius: backgroundItem.bottomLeftRadius
    property alias bottomRightRadius: backgroundItem.bottomRightRadius

    spacing: _units.half

    states: [
        State {
            when: control.down
            PropertyChanges {
                target: control
                backgroundColor: _color.background_overlay_80
            }
        },
        State {
            when: control.hovered && !control.checked && control.enabled
            PropertyChanges {
                target: control
                backgroundColor: _color.background_overlay_40
            }
        },
        State {
            when: control.checked
            PropertyChanges {
                target: control
                backgroundColor: _color.background_overlay_80
            }
        }
    ]

    property color textColor: _color.si_text_dark

    icon.color: textColor

    // implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
    // implicitContentWidth + leftPadding + rightPadding)
    // implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
    // implicitContentHeight + topPadding + bottomPadding)

    width: 32
    height: 32

    topPadding: 2
    bottomPadding: 2
    leftPadding: 4
    rightPadding: 4

    background: Rectangle {
        id: backgroundItem
        anchors{
            fill: parent
        }
        color: control.backgroundColor
        opacity: control.enabled ? 1.0 : 0.3
    }

    contentItem: IconLabel {
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display
        icon: control.icon
        text: control.text
        font: control.font
        color: control.textColor
        opacity: control.enabled ? 1.0 : 0.3
        alignment: Qt.AlignLeft
    }
}
