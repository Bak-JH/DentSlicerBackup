import QtQuick 2.0

Rectangle {
    width : 260
    height : 364

    color: "transparent"

    state : "open"

    /************************** upper **************************/
    Rectangle{
        id : tab
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
        id : content
        width: parent.width
        height: parent.height - 24
        anchors.left : parent.left
        anchors.top :  tab.bottom

        color:"blue"

        Rectangle{
            width : 232
            height: 200

            anchors.top : content.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 12

            color : "#F9F9F9"
        }
    }



    states: [
        State{
            name:"open"
            PropertyChanges { target: arrow; rotation:0 }
            PropertyChanges { target: content; visible:true }
            PropertyChanges { target: ltpl; height : 364 }
        },
        State{
            name:"close"
            PropertyChanges { target: arrow; rotation:180 }
            PropertyChanges { target: content; visible:false }
            PropertyChanges { target: ltpl; height : 24 }
        }
    ]

}
