import QtQuick
import QtQuick.Controls

import Tire 1.0

TabButton {
    id: control

    readonly property var _fonts: Appearence.fonts
    readonly property var _gaps: Appearence.gaps
    readonly property var _radius: Appearence.radius
    readonly property var _color: Appearence.colors

    property color backgroundColor: _color.background_overlay_40

    property alias topLeftRadius: backgroundItem.topLeftRadius
    property alias topRightRadius: backgroundItem.topRightRadius

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

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: textColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    // Custom Background & Indicator
    background: Rectangle {
        id: backgroundItem
        implicitWidth: 100
        implicitHeight: 40

        color: control.backgroundColor
    }
}
