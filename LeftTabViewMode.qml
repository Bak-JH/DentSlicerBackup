import QtQuick 2.0

import QtQuick.Controls 1.2
import QtQuick.Layouts 1.3

Rectangle {
    width : 260
    height : 180

    color : "transparent"

    state : "open"

    Rectangle{
        id : tab
        width: parent.width
        height: 24
        anchors.left : parent.left
        anchors.top :  parent.top

        color: "#CECECE"

        Text{
            text : "View Mode"

            color:"#303030"
            font.family: "Arial"
            font.pointSize: 12

            anchors.verticalCenter: parent.verticalCenter
            anchors.left : parent.left
            anchors.leftMargin: 10
        }

        Image{
            id : arrow
            source: "qrc:/resource/triangle.png"
            width : 12
            height : 10
            //transform:Rotation{origin.x: arrow.width/2; origin.y: arrow.height/2; axis { x: 1; y: 0; z: 0 } angle: 0 }
            rotation: 0

            anchors.verticalCenter: parent.verticalCenter
            anchors.right : parent.right
            anchors.rightMargin: 12

            MouseArea {
                    anchors.fill: parent
                    onClicked: { ltvm.state == 'open' ? ltvm.state = "close" : ltvm.state = 'open';}
            }
        }
    }

    Rectangle{
        id : content
        width: parent.width
        height: parent.height - 24
        anchors.left : parent.left
        anchors.top :  tab.bottom

        color:"Transparent"
        GroupBox {
            //title: "Tab Position"
            width: parent.width
            height: parent.height
            anchors.left: parent.left
            anchors.top : parent.top

            ColumnLayout {
                spacing: 10
                ExclusiveGroup { id: tabPositionGroup }
                RadioButton {
                    text: "Object View"
                    checked: true
                    exclusiveGroup: tabPositionGroup
                }
                RadioButton {
                    text: "Support View"
                    exclusiveGroup: tabPositionGroup
                }
                RadioButton {
                    text: "Layer View"
                    exclusiveGroup: tabPositionGroup
                }
            }
        }
    }

    states: [
        State{
            name:"open"
            PropertyChanges { target: arrow; rotation:0 }
            PropertyChanges { target: content; visible:true }
            PropertyChanges { target: ltvm; height : 180 }

        },
        State{
            name:"close"
            PropertyChanges { target: arrow; rotation:180 }
            PropertyChanges { target: content; visible:false }
            PropertyChanges { target: ltvm; height : 24 }
        }
    ]


}
