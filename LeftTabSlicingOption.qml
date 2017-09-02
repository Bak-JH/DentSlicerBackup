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
            text : "Slicing Option"

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
                    onClicked: { ltso.state == 'open' ? ltso.state = "close" : ltso.state = 'open';}
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
        Item {
            width: parent.width
            height: parent.height
            anchors.left: parent.left
            anchors.top : parent.top
            //anchors.leftMargin: 16
            anchors.topMargin: 18

            ColumnLayout {
                spacing: 8
                LeftTabSlicingOptionElement{
                    columnName: "Resolution"
                    columnContents: ["1920*1080","1024*768"]

                }
                LeftTabSlicingOptionElement{
                    columnName: "Layer height"
                    columnContents: ["0.1","0.2","0.05"]
                }
                LeftTabSlicingOptionElement{
                    columnName: "Support"
                    columnContents: ["k-branch","general"]
                }
                LeftTabSlicingOptionElement{
                    columnName: "Infill"
                    columnContents: ["general","full","none"]
                }
                LeftTabSlicingOptionElement{
                    columnName: "Raft"
                    columnContents: ["general","none"]
                }

            }
        }
    }

    states: [
        State{
            name:"open"
            PropertyChanges { target: arrow; rotation:0 }
            PropertyChanges { target: content; visible:true }
            PropertyChanges { target: ltso; height : 180 }

        },
        State{
            name:"close"
            PropertyChanges { target: arrow; rotation:180 }
            PropertyChanges { target: content; visible:false }
            PropertyChanges { target: ltso; height : 24 }
        }
    ]


}
