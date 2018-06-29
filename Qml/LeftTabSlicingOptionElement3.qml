import QtQuick 2.4

import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import QtQuick.Controls.Private 1.0

Item {
    width: parent.width
    height: 24

    property string density : numberBox_text.text


    Text{
        id:text
        width: 116
        height: parent.height
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.verticalCenter: parent.verticalCenter

        font.pixelSize: 14
        //font.bold: true
        anchors.leftMargin:14
        //text: columnName
        font.family: mainFont.name
        verticalAlignment: Text.AlignBottom
        color : "black"
    }

    Item {
        id: numberBoxItem
        width: 116
        height: parent.height
        anchors.top: parent.top
        anchors.left : text.right
        property alias numberBox_text: numberBox_text
        //number box
        Rectangle {
            id: numberBox
            width: parent.width
            height: parent.height
            color: "#F9F9F9"
            property alias numberBox_text: numberBox_text
            Text{
                width: parent.width
                height: parent.height
                anchors.centerIn: parent

                text : ""

                font.pixelSize: 14
                font.family: mainFont.name
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                color: "#0DA3B2"
            }
            TextInput {
                id: numberBox_text
                anchors.left: parent.left
                anchors.leftMargin: 16
                anchors.verticalCenter: parent.verticalCenter
                text: "50%"
                maximumLength: 4
                //maximumLength:
                font.pixelSize: 14
                font.family: mainFont.name
                color: focus ? "black" : "black"
                selectByMouse: true
                onEditingFinished:  {
                    if(!numberBox_text.text.toString().includes("%"))
                        numberBox_text.text = numberBox_text.text+"%"
                    density = numberBox_text.text
                }

            }

        }
    }


}
