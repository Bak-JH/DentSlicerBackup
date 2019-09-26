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
        id : elemnt3_text
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
        anchors.left : elemnt3_text.right
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
                maximumLength: 7
                text: "50%"
                font.pixelSize: 14
                font.family: mainFont.name
                //color: focus ? "black" : "black"
                selectByMouse: true
                onEditingFinished:  {
                    if(!numberBox_text.text.toString().includes("%"))
                        numberBox_text.text = numberBox_text.text+"%"
                    density = numberBox_text.text
                }

                onTextChanged: {
                    console.log("text   " + text);

                    if (text.length === 1){
                        text = text.replace(/[^0-9.]\%/g,'');
                    }else{
                        text = text.charAt(0).replace(/[^1-9.]\%/g,'')+text.substring(1,text.length).replace(/[^0-9.]\%/g, '');
                    }

                    if (parseFloat(text) > 100){
                        text = text.slice(0,-1)
                    }
                }

            }

        }
    }


}
