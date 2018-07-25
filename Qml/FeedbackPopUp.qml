import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Scene3D 2.0
import QtQuick.Window 2.2
import QtCanvas3D 1.1
import QtQuick.Controls.Styles 1.4
import "glcode.js" as GLCode
import QtQuick.Dialogs 1.2
import QtWinExtras 1.0
Item {
    width: 356
    height: 440

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

        Image{
            id : logo
            width : 136
            height: 86
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 60

            source: "qrc:/Resource/login_logo.png"
        }


        Text{
            id : text1
            width: parent.width
            height: 20
            anchors.top: logo.bottom
            anchors.topMargin: 50

            text : "Thank you for using " + "<font color=\"#0DA3B2\">DentStudio Beta</font>"

            font.pixelSize: 20
            font.family: mainFont.name
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: "#666666"
        }
        Text{
            id : text2
            width: parent.width
            height: 20
            anchors.top: text1.bottom
            anchors.topMargin: 30

            text : "If there is bug, unpredictable movement"

            font.pixelSize: 16
            font.family: mainFont.name
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: "#666666"
        }
        Text{
            id : text3
            width: parent.width
            height: 20
            anchors.top: text2.bottom
            anchors.topMargin: 8

            text : "and suggestion for " + "<font color=\"#0DA3B2\">DentStudio</font>"

            font.pixelSize: 16
            font.family: mainFont.name
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: "#666666"
        }

        Text{
            id : text4
            width: parent.width
            height: 20
            anchors.top: text3.bottom
            anchors.topMargin: 20

            text : "Please send email to " + "<font color=\"#0DA3B2\">contact@hix.co.kr</font>"

            font.pixelSize: 16
            font.family: mainFont.name
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: "#666666"
        }


        Rectangle { // login button
            width: 284
            height: 36
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.leftMargin: 36
            anchors.bottomMargin: 36
            color: "transparent"
            Image {
                anchors.fill: parent
                source: "qrc:/Resource/login_leftbutton.png"
            }
            Text{
                width: parent.width
                height: parent.height
                anchors.centerIn: parent

                text : "OK"

                font.pixelSize: 20
                font.family: mainFont.name
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: "white"
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered : qm.setHandCursor();
                onExited : qm.resetCursor();
                onClicked: {
                    feedbackPopUp.visible = false
                }
            }
        }

    }

    function closePopUp(){
        feedbackPopUp.visible = false
    }
}
