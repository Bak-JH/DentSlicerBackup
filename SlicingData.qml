import QtQuick 2.0

Item {
    width: 360
    height: 64

    Rectangle{
        anchors.fill: parent
        color: "white"
        border.width: 1
        border.color: "#E0E0E0"
        radius: 1
    }

    Rectangle{
        id:icon
        width:60
        height: 48
        anchors.left : parent.left
        anchors.top : parent.top
        anchors.topMargin: 10

        color: "transparent"

        Image{
            width: 34
            height: 34
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            source: "qrc:/resource/slicing_calculate.png"
        }

        Text{
            text:"Calculate"
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter

            font.family: "Arial"
            font.pixelSize: 10
            verticalAlignment: Text.AlignBottom

        }


    }
    Rectangle{
        id:data
        width: 288
        height: 46
        anchors.left : icon.right
        anchors.top : parent.top
        anchors.topMargin: 10

        color: "transparent"

        border.width: 1
        border.color: "#E0E0E0"
        /*
        Item{
            id:left
            width: 176
            height: parent.height
            anchors.left: parent.left
            anchors.top : parent.top

            Item{

            }

        }

        Item{
            id:right
            width: 100
            height: parent.height

        }*/
    }

}
