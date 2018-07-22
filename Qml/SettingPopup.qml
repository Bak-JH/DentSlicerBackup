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
    height: 436

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

        Text{
            id : popup_title
            width: 100
            height: 20
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 28
            anchors.leftMargin: 28

            text : "Setting"

            font.pixelSize: 20
            font.family: mainFont.name
            font.bold: true
            verticalAlignment: Text.AlignVCenter
            color: "#666666"
        }

        Rectangle{
            id : box_language
            width: 278
            height: 56
            anchors.top: popup_title.bottom
            anchors.left: parent.left
            anchors.topMargin: 24
            anchors.leftMargin: 28

            color: "transparent"

            Text{
                width: parent.width
                height: 20
                anchors.top: parent.top
                anchors.left: parent.left

                text : "Language"

                font.pixelSize: 16
                font.family: mainFont.name
                verticalAlignment: Text.AlignVCenter
                color: "#0DA3BE"
            }
            Rectangle{
                width: 256
                height: 24
                anchors.bottom: parent.bottom
                anchors.right: parent.right

                color: "#E2E2E2"
            }

        }

        Rectangle{
            id : box_software_information
            width: 298
            height: 180
            anchors.top: box_language.bottom
            anchors.left: parent.left
            anchors.topMargin: 32
            anchors.leftMargin: 28

            color: "transparent"

            Text{
                width: parent.width
                height: 20
                anchors.top: parent.top
                anchors.left: parent.left

                text : "Software Information"

                font.pixelSize: 16
                font.family: mainFont.name
                verticalAlignment: Text.AlignTop
                color: "#0DA3BE"
            }

            Rectangle{
                width: parent.width
                height: 150
                anchors.bottom: parent.bottom
                color: "transparent"

                border.width: 1
                radius: 1
                border.color: "#999999"

                Text{
                    width: 120
                    height: 20
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.leftMargin: 18
                    anchors.topMargin: 14


                    text : "Current Version : "

                    font.pixelSize: 14
                    font.family: mainFont.name
                    color: "#696B70"
                }

                Text{
                    width: 120
                    height: 20
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.leftMargin: 18
                    anchors.topMargin: 38


                    text : "Latest Version : "

                    font.pixelSize: 14
                    font.family: mainFont.name
                    color: "#696B70"
                }

                Text{
                    id : current_version_text
                    width: 120
                    height: 20
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.rightMargin: 18
                    anchors.topMargin: 14


                    text : "v1.1.1"

                    font.pixelSize: 14
                    font.family: mainFont.name
                    horizontalAlignment: Text.AlignRight
                    color: "#0DA3BE"
                }

                Text{
                    id : latest_version_text
                    width: 120
                    height: 20
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.rightMargin: 18
                    anchors.topMargin: 38


                    text : "v1.1.1"

                    font.pixelSize: 14
                    font.family: mainFont.name
                    horizontalAlignment: Text.AlignRight
                    color: "#0DA3BE"
                }

                Text{
                    width: 200
                    height: 20
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.leftMargin: 18
                    anchors.topMargin: 62


                    text : "You are using the latest version."

                    font.pixelSize: 14
                    font.family: mainFont.name
                    color: "#191919"
                }

                Rectangle{
                    width: 134
                    height: 30

                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.rightMargin: 18
                    anchors.bottomMargin: 20

                    color : "#BBB"

                    Text{
                        width : parent.width
                        height: parent.height
                        anchors.centerIn: parent

                        font.pixelSize: 16
                        font.family: mainFont.name
                        text: "Update Software"
                        color: "white"

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }

        }





        Rectangle { // login button
            width: 298
            height: 36
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 26

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
                    settingPopup.visible = false
                }
            }
        }

    }
}
