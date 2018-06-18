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
    width: 262
    height: 152

    anchors.left: parent.left
    anchors.top: parent.top
    anchors.leftMargin: 280
    anchors.topMargin: 120

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
            id : text1
            x: 15
            y: 15
            width: 300
            height: 20
            anchors.topMargin: 50

            font.pixelSize: 20
            font.family: mainFont.name
            font.bold: true
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            color: "#666666"
            text: "Color scheme"
        }

        CheckBox {
            id: checkBox
            x: 30
            y: 60
            width: 212
            height: 19
            text: qsTr("Infill")
            checked: false

            Rectangle {
                id: rectangle
                x: 185
                y: 0
                width: 19
                height: 19
                color: "#f7f12f"
                radius: 10
                border.width: 0
                border.color: "#f7f12f"
            }

            Rectangle {
                id: rectangle1
                x: 185
                y: 25
                width: 19
                height: 19
                color: "#42bfcc"
                radius: 10
                border.color: "#42bfcc"
                border.width: 0
            }

            Rectangle {
                id: rectangle2
                x: 185
                y: 50
                width: 19
                height: 19
                color: "#6eed32"
                radius: 10
                border.color: "#6eed32"
                border.width: 0
            }
        }

        CheckBox {
            id: checkBox1
            x: 30
            y: 85
            width: 212
            height: 19
            text: qsTr("Supporters")
        }

        CheckBox {
            id: checkBox2
            x: 30
            y: 110
            width: 212
            height: 19
            text: qsTr("Raft")
        }


    }
}
