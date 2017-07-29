import QtQuick 2.0

Rectangle {
    width : 260
    height : 364

    color: "transparent"

    state : "open"

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
            id : arrow
            source: "qrc:/Resource/triangle.png"
            width : 12
            height : 10

            anchors.verticalCenter: parent.verticalCenter
            anchors.right : parent.right
            anchors.rightMargin: 12

            MouseArea {
                    anchors.fill: parent
                    onClicked: { ltpl.state == 'open' ? ltpl.state = "close" : ltpl.state = 'open';}
            }
        }

    }

    /************************** content **************************/
    Rectangle{
        width : 232
        height: 200

        anchors.top : name.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 12

        color : "#F9F9F9"
    }

    states: [
        State{
            name:"open"
            PropertyChanges { target: arrow; rotation:0 }
        },
        State{
            name:"close"
            PropertyChanges { target: arrow; rotation:180 }
        }
    ]

}
