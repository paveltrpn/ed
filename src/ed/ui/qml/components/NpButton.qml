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

    property alias backgroundRadius: backgroundItem.radius

    property bool isBorderEnabled: false
    property color backgroundColor: _color.background_overlay_light_60

    opacity: enabled ? 1.0 : 0.3

    states: [
        State {
            when: control.down
            PropertyChanges {
                target: control
                backgroundColor: _color.background_overlay_light
            }
        },
        State {
            when: control.hovered && !control.checked && control.enabled
            PropertyChanges {
                target: control
                backgroundColor: _color.background_overlay_light_80
            }
        },
        State {
            when: control.checked
            PropertyChanges {
                target: control
                backgroundColor: _color.background_overlay_light
            }
        }
    ]

    property color textColor: _color.main_contrast

    // icon.color: textColor
    // implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
    // implicitContentWidth + leftPadding + rightPadding)
    // implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
    // implicitContentHeight + topPadding + bottomPadding)

    width: 32
    height: 32

    topPadding: 2
    bottomPadding: 2
    leftPadding: 2
    rightPadding: 2

    background: Rectangle {
        id: backgroundItem
        anchors.fill: parent
        color: control.backgroundColor
        opacity: control.enabled ? 1.0 : 0.3
        radius: _units.radiusEight
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
    }
}
