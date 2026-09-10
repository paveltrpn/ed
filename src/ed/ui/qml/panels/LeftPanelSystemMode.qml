// qmllint disable unqualified
// qmllint disable Quick.property-changes-parsed

import QtQuick
import QtQuick.Layouts

import Tire 1.0

import "../components"
import "../sceneinfo"
import "../addforms"
import "../settings"

Item {
    id: leftPanelSystemComponent

    readonly property var _color: Appearence.colors
    readonly property var _fonts: Appearence.fonts
    readonly property var _units: Appearence.units

    property var buttonsList: []

    function deselectOthers(exceptItem) {
        for (let i in leftPanelSystemComponent.buttonsList) {
            const btn = leftPanelSystemComponent.buttonsList[i];
            if (btn !== exceptItem) {
                btn.checked = false;
            }
        }
    }

    Rectangle {
        id: leftPanelMainComponentWrapper
        anchors.fill: parent
        color: _color.si_button_shadow

        SIButtonMain {
            id: aboutButton

            anchors {
                top: parent.top
                topMargin: leftPanelSystemComponent._units.half
                left: parent.left
                leftMargin: leftPanelSystemComponent._units.half
                right: parent.right
                rightMargin: leftPanelSystemComponent._units.half
            }

            height: _units.scaled_56

            buttonLabel: "About"

            onClicked: {
                aboutButton.checked = !aboutButton.checked;

                if (aboutButton.checked) {
                    leftPanelSystemComponent.deselectOthers(aboutButton);
                }
            }

            Component.onCompleted: {
                leftPanelSystemComponent.buttonsList.push(aboutButton);
            }
        }

        SIButtonMain {
            id: exitButton

            anchors {
                bottom: parent.bottom
                left: parent.left
                leftMargin: leftPanelSystemComponent._units.half
                right: parent.right
                rightMargin: leftPanelSystemComponent._units.half
            }

            height: _units.scaled_56

            buttonLabel: "Exit"

            onClicked: {
                exitButton.checked = !exitButton.checked;

                if (exitButton.checked) {
                    leftPanelSystemComponent.deselectOthers(exitButton);
                }
            }

            Component.onCompleted: {
                leftPanelSystemComponent.buttonsList.push(exitButton);
            }
        }
    }
}
