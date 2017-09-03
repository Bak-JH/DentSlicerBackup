import QtQuick 2.0

import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3


Item {
    width: parent.width
    height: 24

    property string columnName
    property var columnContents :[]



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

            background: Rectangle {
                radius:1
                color: "#F9F9F9"
            }
            label: Text {
                verticalAlignment: Text.AlignVCenter
                anchors.left: parent.left
                anchors.leftMargin: 10

                font.pixelSize: 14
                font.family: "Arial"
                //font.family: "Courier"
                //font.capitalization: Font.SmallCaps
                color: "black"
                text: control.currentText
            }
        }
    }

}
