import QtQuick 2.0

Rectangle {
    width : 256
    height : 800

    color: "transparent"

    LeftTabPartList{
        id : lefttab1
    }

    BlurRectangle{
        id : lefttab2
        inputWidth: 256
        inputHeight: 50
        inputRadius: 6

        anchors.top: lefttab1.bottom
        anchors.left: parent.left
        anchors.topMargin: 20

        Text{
            width: 160
            height : 20

            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 16

            text : "View Mode"

            color: "#888888"
            font.family: "Helvetica"
            verticalAlignment: Text.AlignVCenter
            font.pointSize: 16
        }

        Image {
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: 15
            source: "qrc:/Resource/lefttab-triangle.png"
        }

    }

    BlurRectangle{
        id : lefttab3
        inputWidth: 256
        inputHeight: 50
        inputRadius: 6

        anchors.top: lefttab2.bottom
        anchors.left: parent.left
        anchors.topMargin: 20

        Text{
            width: 160
            height : 20

            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 16

            text : "Slicing Option"

            color: "#888888"
            font.family: "Helvetica"
            verticalAlignment: Text.AlignVCenter
            font.pointSize: 16
        }

        Image {
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: 15
            source: "qrc:/Resource/lefttab-triangle.png"
        }


    }

    BlurRectangle{
        id : lefttab4
        inputWidth: 256
        inputHeight: 250
        inputRadius: 8

        inputColor : "#F6FEFF"

        anchors.top: lefttab3.bottom
        anchors.left: parent.left
        anchors.topMargin: 20
    }

}
