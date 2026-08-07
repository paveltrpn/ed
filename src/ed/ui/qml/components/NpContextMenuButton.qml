// qmllint disable unqualified
// qmllint disable Quick.property-changes-parsed

import QtQuick
import QtQuick.Templates as QuickTemplates
import QtQuick.Controls.impl

import Tire 1.0

QuickTemplates.Button {
    id: control

    readonly property var _fonts: Appearence.fonts
    readonly property var _gaps: Appearence.gaps
    readonly property var _radius: Appearence.radius
    readonly property var _color: Appearence.colors

    property bool isBorderEnabled: false
    property color backgroundColor: _color.background_overlay_40

    states: [
        State {
            when: control.down
            PropertyChanges {
                target: control
                backgroundColor: _color.additional_contrast_80
            }
        },
        State {
            when: control.hovered && !control.checked
            PropertyChanges {
                target: control
                backgroundColor: _color.background_overlay_light_60
            }
        },
        State {
            when: control.checked
            PropertyChanges {
                target: control
                backgroundColor: _color.additional_contrast_80
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
    height: 128

    topPadding: 2
    bottomPadding: 2
    leftPadding: 2
    rightPadding: 2

    background: Rectangle {
        id: backgroundItem
        anchors.fill: parent
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
