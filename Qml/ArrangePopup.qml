import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Rectangle {
    width: 360
    height: 220
    anchors.centerIn: resultPopUp
    visible : false

    color: "#E5E5E5"
    border.width: 1
    border.color:"#CCCCCC"

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
            id: result_text

            text: "Click Apply to auto-arrange models."
            anchors.top: parent.top
            anchors.topMargin: 28
            anchors.horizontalCenter: parent.horizontalCenter
            verticalAlignment: Text.AlignTop
            font.pixelSize: 15
            color: "#0DA3B2"
            font.family: mainFont.name
        }

        Image {
            width: 178
            height: 76
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            anchors.topMargin: 10

            source: "qrc:/Resource/popup_image/image_arrange.png"
        }


        Rectangle {
            id: applyButton
            width: 132
            height: 30
            color: "#A3A3A5"
            anchors.left : parent.left
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 22
            anchors.leftMargin: 32


            Text {
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Apply"
                color: "#ffffff"
                font.pixelSize: 16
                font.family: mainFont.name
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered:{
                    qm.setHandCursor();
                    parent.color = "#3ea6b7"
                }
                onExited:{
                    qm.resetCursor();
                    parent.color = "#999999"
                }
                onReleased: {
                    qm.runArrange();
                    closePopUp();
                    uppertab.all_off();
                }
            }
        }

        Rectangle {
            id: cancelButton
            width: 132
            height: 32
            color: "#A3A3A5"
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 22
            anchors.rightMargin: 32

            Text {
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Cancel"
                color: "#ffffff"
                font.pixelSize: 16
                font.family: mainFont.name
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered:{
                    qm.setHandCursor();
                    parent.color = "#3ea6b7"
                }
                onExited:{
                    qm.resetCursor();
                    parent.color = "#999999"
                }
                onReleased: {
                    closePopUp();
                    uppertab.all_off();
                }
            }
        }
    }


    function closePopUp(){
        arrangePopUp.visible = false
    }


}
