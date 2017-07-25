import QtQuick 2.0

Rectangle {
    width : 260
    height : 364

    color: "transparent"

    /************************** upper **************************/
    Rectangle{
        id : name
        width: parent.width
        height: 24
        anchors.left : parent.left
        anchors.top :  parent.top

        color: "#CECECE"

        Text{
            text : "Part List"

            color:"#303030"
            font.family: "Arial"
            font.pointSize: 12

            anchors.verticalCenter: parent.verticalCenter
            anchors.left : parent.left
            anchors.leftMargin: 10
        }

        Image{
            source: "qrc:/Resource/triangle.png"
            width : 12
            height : 10

            anchors.verticalCenter: parent.verticalCenter
            anchors.right : parent.right
            anchors.rightMargin: 12

            MouseArea {
                    anchors.fill: parent
                    onClicked: { console.log("Part List") }
            }
        }

    }

    /************************** content **************************/
    Rectangle{
        width : 232
        height: 200

        anchors.top : name.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 12

        color : "#F9F9F9"
    }

}
