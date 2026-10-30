// qmllint disable unqualified
// qmllint disable Quick.property-changes-parsed

import QtQuick
import QtQuick.Layouts

import Tire 1.0

import "../components"
import "../sceneinfo"
import "../addforms"
import "../settings"

Rectangle {
    id: leftPanelRenderComponent

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    color: _color.si_button_shadow

    property string modeColor: _color.si_mode_render

    Item {
        id: leftPanelMainComponentWrapper
        anchors {
            fill: parent
            leftMargin: leftPanelRenderComponent._units.scaled_2
            rightMargin: leftPanelRenderComponent._units.scaled_2
        }

        property string activeBtn: ""

        MouseArea {
            id: closePopupsMouseArea
            anchors.fill: parent
            onClicked: {
                mainWindow.panelsActiveBtn = "";
            }
        }

        SIButtonMain {
            id: tcpBridgeTestStartButton

            anchors {
                top: parent.top
                topMargin: leftPanelRenderComponent._units.quarter
                left: parent.left
                right: parent.right
            }

            modeRelatedBgColor: leftPanelRenderComponent.modeColor
            buttonLabel: "TCP test start"

            onClicked: {
                Tired.renderRequest.start()
            }
        }
    }
}
