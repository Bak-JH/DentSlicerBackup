import QtQuick 2.0

Item {
    width : 194
    height: 262


    Item{
        width: 48
        height: 50
        anchors.top : parent.top
        anchors.right : parent.right

        //color: "red"
        Image{
            anchors.fill: parent
            source:"qrc:/resource/view.png"
        }
        Text{
            text:"VIEW"
            anchors.top : parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 8

            font.family: "Arial"
            font.pixelSize: 12

        }
    }
    Image{
        width : parent.width
        height : 200

        anchors.bottom: parent.bottom

        source:"qrc:/resource/view_background.png"

    }


}
