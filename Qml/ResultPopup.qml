import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Rectangle {
    objectName: "result_popup"
    width: 352
    height: 162
    anchors.centerIn: parent
    visible : false

    color: "#E5E5E5"
    border.width: 1
    border.color:"#CCCCCC"

    property string high_text: high_text
    property string mid_text: mid_text
    property string low_text: low_text

    Rectangle{//shadow
        id : shadowRect
        width: parent.width-2
        height: parent.height-2
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        color: "#CCCCCC"
    }

    Rectangle{//main
        id : contentRect
        width: parent.width-2
        height: parent.height-2
        anchors.top: parent.top
        anchors.left: parent.left

        color: "#ffffff"

        Text {
            height:20
            id: result_text_high

            text: high_text
            anchors.top: parent.top
            anchors.topMargin: 30
            anchors.horizontalCenter: parent.horizontalCenter
            verticalAlignment: Text.AlignTop
            font.pixelSize: 16
            color: "#0DA3B2"
            font.family: mainFont.name
        }

        Text {
            height:20
            id: result_text_mid

            text: mid_text
            anchors.top: parent.top
            anchors.topMargin: 44
            anchors.horizontalCenter: parent.horizontalCenter
            verticalAlignment: Text.AlignTop
            font.pixelSize: 16
            color: "#0DA3B2"
            font.family: mainFont.name
        }

        Text {
            height:20
            id: result_text_low

            text: low_text
            anchors.top: parent.top
            anchors.topMargin: 58
            anchors.horizontalCenter: parent.horizontalCenter
            verticalAlignment: Text.AlignTop
            font.pixelSize: 16
            color: "#0DA3B2"
            font.family: mainFont.name
        }

        Rectangle {
            id: okButton
            width: 138
            height: 34
            color: "#A3A3A5"
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 23
            anchors.horizontalCenter: parent.horizontalCenter

            Text {
                id: okButton_text
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                text: "OK"
                color: "#ffffff"
                font.pixelSize: 20
                font.family: mainFont.name
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    qm.setHandCursor();
                    parent.color = "#3ea6b7"
                }
                onExited: {
                    qm.resetCursor();
                    parent.color = "#999999"
                }
                onReleased: {
                    closePopUp();
                    //uppertab.all_off();
                    if(mid_text.includes("at least one model") || mid_text.includes("change to"))
                        uppertab.all_off()
                    if (mid_text.includes("Saved")) {
                        qm.closeSave()
                        uppertab.all_off()
                    }
                }
            }
        }
    }


    function openResultPopUp(inputText_h, inputText_m, inputText_l){
        resultPopUp.visible = true
        high_text = inputText_h
        mid_text = inputText_m
        low_text = inputText_l

    }

    function closePopUp(){
        resultPopUp.visible = false
    }


}
