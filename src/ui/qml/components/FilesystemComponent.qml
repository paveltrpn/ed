// qmllint disable unqualified
// qmllint disable Quick.property-changes-parsed

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Tire 1.0

import ".."

Rectangle {
    id: filesystemComponent

    readonly property var _color: Appearence.colors.data
    readonly property var _fonts: Appearence.fonts.data
    readonly property var _units: Appearence.units.data

    color: "transparent"

    anchors.fill: parent

    signal closeButtonClicked

    Rectangle {
        id: filepathPanel

        anchors {
            left: parent.left
            right: buttonsPanel.left
            rightMargin: _units.scaled_4
            top: parent.top
        }

        height: _units.scaled_32

        border {
            width: 1
            color: _color.si_background_dark
        }

    }

    Rectangle {
        id: filesListPanel

        anchors {
            left: parent.left
            top: filepathPanel.bottom
            topMargin: _units.scaled_4
            bottom: parent.bottom
        }

        width: _units.scaled_496

        border {
            width: 1
            color: _color.si_background_dark
        }

        FilesystemItem {
            id: filesystemItem

            anchors.fill: parent

            ListView {
                anchors {
                    fill: parent
                    margins: 4
                }

                clip: true

                model: filesystemItem.filesystemModel

                delegate: Rectangle {

                    color: dirEntryItemArea.containsMouse ? _color.si_background_light_faded : "transparent"

                    width: ListView.view.width
                    height: _units.scaled_18

                    MouseArea {
                        id: dirEntryItemArea
                        anchors.fill: parent
                        hoverEnabled: true

                        onDoubleClicked: {
                            const item = filesystemItem.filesystemModel.get(index)
                            console.log(" === " + item.filePath)
                        }
                    }

                    Text {
                        id: dirEntryText

                        anchors {
                            top: parent.top
                            bottom: parent.bottom
                            left: parent.left
                            leftMargin: _units.scaled_4
                        }

                        width: implicitWidth

                        color: _color.si_text_dark

                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter

                        font: _fonts.text_body

                        text: model.fileName
                    }
                }
            }

            // TreeView {
            //     id: treeView
            //     anchors {
            //         top: parent.top
            //         bottom: parent.bottom
            //         left: parent.left
            //         margins: 4
            //     }

            //     width: 128

            //     model: filesystemItem.filesystemModel
            //     // rootIndex must be a QModelIndex; convert path -> index
            //     // rootIndex: filesystemItem.index(filesystemItem.filesystemModel.rootPath())

            //     selectionModel: ItemSelectionModel {
            //         model: filesystemItem
            //     }

            //     delegate: Rectangle {
            //         width: treeView.width
            //         implicitHeight: 26
            //         color: treeView.currentIndex === model.index ? palette.highlight : "transparent"

            //         required property TreeView treeView
            //         required property bool isTreeNode
            //         required property bool expanded
            //         required property bool hasChildren
            //         required property int depth
            //         required property int row
            //         required property int column
            //         required property var model            // the underlying QModelIndex (role 0 = display)

            //         // expand arrow
            //         Text {
            //             visible: hasChildren
            //             x: depth * 20 + 4
            //             anchors.verticalCenter: parent.verticalCenter
            //             text: expanded ? "▾" : "▸"

            //             MouseArea {
            //                 anchors.fill: parent
            //                 onClicked: treeView.toggle(expandedIndex)
            //             }
            //         }

            //         Text {
            //             anchors.verticalCenter: parent.verticalCenter
            //             x: depth * 20 + (hasChildren ? 22 : 6)
            //             text: model.display
            //             elide: Text.ElideMiddle
            //             width: parent.width - x - 8
            //         }

            //         property var expandedIndex: model.index
            //     }
            // }
        }
    }

    Rectangle {
        id: buttonsPanel

        anchors {
            left: filesListPanel.right
            leftMargin: _units.scaled_4
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }

        border {
            width: 1
            color: _color.si_background_dark
        }

        SIButton {
            id: doButton

            anchors {
                left: parent.left
                leftMargin: _units.scaled_8
                right: parent.right
                rightMargin: _units.scaled_8
                bottom: closeButton.top
                bottomMargin: _units.scaled_8
            }

            height: _units.scaled_24

            buttonLabel: "Do"

            onClicked: {
                siEmbeddedWidgetComponent.aboutToClose();
            }
        }

        SIButton {
            id: closeButton

            anchors {
                left: parent.left
                leftMargin: _units.scaled_8
                right: parent.right
                rightMargin: _units.scaled_8
                bottom: parent.bottom
                bottomMargin: _units.scaled_8
            }

            height: _units.scaled_24

            buttonLabel: "Close"

            onClicked: {
                siEmbeddedWidgetComponent.aboutToClose();
            }
        }
    }
}
