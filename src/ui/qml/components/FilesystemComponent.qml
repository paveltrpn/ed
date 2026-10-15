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

    FilesystemItem {
        id: filesystemItem

//        anchors.fill: parent

        width: 128
        height: 128

        ListView {
            anchors {
                top: parent.top
                bottom: parent.bottom
                left: parent.left
                margins: 4
            }

            width: 128

            model: filesystemItem.filesystemModel

            delegate: Rectangle {
                color: "red"
                width: 128
                height: 16

                Text {
                    id: dirEntryText

                    anchors {
                        left: parent.left
                        leftMargin: _units.scaled_4
                    }

                    height: 16
                    width: implicitWidth

                    color: _color.si_text_dark

                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter

                    font: _fonts.text_body

                    text: model.display
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
