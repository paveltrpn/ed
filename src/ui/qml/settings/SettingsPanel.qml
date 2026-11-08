// qmllint disable unqualified

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0
import "../components"

Rectangle {
    id: settingsPanelComponent

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    color: _color.si_background_light
    radius: _units.radiusEight
    height: visible ? settingsPanelButtonsLayout.implicitHeight : 0

    signal close

    Behavior on height {
        NumberAnimation {
            duration: 50
        }
    }

    ColumnLayout {
        id: settingsPanelButtonsLayout
        spacing: 0

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        readonly property int itemsHeight: _units.scaled_24

        NpContextMenuButton {
            id: serviceObjectsSettingsButton
            Layout.fillWidth: true
            Layout.preferredHeight: settingsPanelButtonsLayout.itemsHeight
            text: "Service objects"
            enabled: true

            topLeftRadius: _units.radiusEight
            topRightRadius: _units.radiusEight

            font: _fonts.label_accent

            onClicked: {
                console.log(" ===== ");
                if (!serviceObjectsSettingsWidget.visible) {
                    serviceObjectsSettingsWidget.visible = true;
                }
            }

            SIEmbeddedWidget {
                id: serviceObjectsSettingsWidget

                visible: false

                width: 640
                height: 480

                title: "Service objects settings"

                dragBounds: renderViewWrapper

                onAboutToClose: {
                    serviceObjectsSettingsWidget.visible = false;
                }

                contentSource: "../settings/SettingsComponent.qml"

                x: centeredWidgetPosition(serviceObjectsSettingsWidget).x
                y: centeredWidgetPosition(serviceObjectsSettingsWidget).y
            }
        }
    }
}
