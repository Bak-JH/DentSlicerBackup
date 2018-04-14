import QtQuick 2.0

import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import QtQuick.Controls.Private 1.0

Item {
    width: parent.width
    height: 24

    property string columnName
    property var columnContents :[]

    property alias currentText: combo.currentText

    Text{
        id:text
        width: 116
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter

        font.pixelSize: 14
        font.bold: true
        anchors.leftMargin:16
        text: columnName
        font.family: "Arial"

    }

    ComboBox {
        id: combo
        width: 116
        height: parent.height
        anchors.left: text.right
        anchors.top : parent.top
        model:columnContents

        Image{
            id : arrow

            width: 10
            height: 6
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: 8

            source : "qrc:/resource/combo_arrow.png"
        }



        style: ComboBoxStyle {
            selectedTextColor:{
                color : "red"
            }

            background: Rectangle {
                radius:1
                color: "#F9F9F9"
            }
            label: Text {
                verticalAlignment: Text.AlignVCenter
                anchors.left: parent.left
                anchors.leftMargin: 16

                font.pixelSize: 14
                font.family: "Arial"

                color: "black"
                text: control.currentText
            }


            property Component __dropDownStyle: MenuStyle {
                __maxPopupHeight: 600
                __menuItemType: "comboboxitem"

                frame: Rectangle {              // background
                    color: "#F9F9F9"
                    border.width: 1
                    border.color: "#C6C6C6"
                    //radius: 15
                }

                itemDelegate.label:             // an item text
                    Text
                    {
                        height: 24
                        verticalAlignment: Text.AlignVCenter
                        //horizontalAlignment: Text.AlignHCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 16

                        font.pixelSize: 14
                        font.family: "Arial"

                        color: "black"

                        //font.family: "Courier"
                        //font.capitalization: Font.SmallCaps

                        text: styleData.text
                    }

                itemDelegate.background: Rectangle {  // selection of an item
                    //radius: 5
                    height: 20
                    color: styleData.selected ? "#EAEAEA" : "transparent"
                }

                __scrollerStyle: ScrollViewStyle { }
            }
        }




    }
}
