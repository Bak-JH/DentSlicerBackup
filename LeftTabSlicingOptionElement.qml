import QtQuick 2.0

import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3


Item {
    width: parent.width
    height: 24



    Text{
        id:text
        width: 116
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter

        font.pixelSize: 14
        font.bold: true
        anchors.leftMargin:16
        text:"aaaaa"

    }

    ComboBox {
        width: 116
        height: parent.height
        anchors.left: text.right
        anchors.top : parent.top
        model:["test1","test2","test3"]

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
                //font.family: "Courier"
                //font.capitalization: Font.SmallCaps
                color: "black"
                text: control.currentText
            }
        }
    }

}
