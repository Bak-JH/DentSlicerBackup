import QtQuick 2.0

import QtQuick.Controls 1.2
import QtQuick.Layouts 1.3

Rectangle {
    property alias option_resolution: option_resolution
    property alias option_layer_height: option_layer_height
    property alias option_bed_number: option_bed_number
    property alias option_resin_type: option_resin_type
    property alias option_raft: option_raft
    property alias option_support: option_support
    property alias option_support_density: option_support_density

    property alias option_infill: option_infill
    property alias option_infill_density: option_infill_density


    width : 264
    height : 180

    color : "#efefef"

    state : "open"

    Rectangle{
        id : tab
        width: parent.width
        height: 28
        anchors.left : parent.left
        anchors.top :  parent.top
        z: 10

        color: "#efefef"

        Text{
            height: parent.height
            text : "Slicing Option"

            color:"#888D8E"
            font.family: mainFont.name
            font.pixelSize: 16

            anchors.bottom: parent.bottom
            anchors.left : parent.left
            anchors.leftMargin: 14
            verticalAlignment: Text.AlignBottom
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
                    hoverEnabled : true
                    onEntered : qm.setHandCursor();
                    onExited : qm.resetCursor();
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

        color:"#efefef"
        Item {
            width: parent.width
            height: parent.height
            anchors.left: parent.left
            anchors.top : parent.top

            anchors.topMargin: 16

            ColumnLayout {
                spacing: 2
                LeftTabSlicingOptionElement{
                    id:option_resolution
                    columnName: "Resolution"
                    columnContents: ["1920*1080","1024*768"]
                }

                Item{width:parent.width;height:2}//spacer

                LeftTabSlicingOptionElement{
                    id:option_layer_height
                    columnName: "Layer height"
                    columnContents: ["0.1","0.2","0.05"]
                }

                Item{width:parent.width;height:2}//spacer

                LeftTabSlicingOptionElement{
                    id:option_bed_number
                    columnName: "Bed number"
                    columnContents: ["1","2","3"]
                }

                Item{width:parent.width;height:2}//spacer

                LeftTabSlicingOptionElement{
                    id:option_resin_type
                    columnName: "Resin Type"
                    columnContents: ["Temporary","Clear","Castable"]
                }

                Item{width:parent.width;height:2}//spacer

                LeftTabSlicingOptionElement{
                    id:option_raft
                    columnName: "Raft"
                    columnContents: ["general","none"]

                    Item{width:parent.width;height:2}//spacer

                }
                LeftTabSlicingOptionElement2{
                    columnName: "Support"
                    columnText: "Type"
                }

                LeftTabSlicingOptionElement{
                    id:option_support
                    columnName: ""
                    columnContents: ["k-branch","general"]
                }
                LeftTabSlicingOptionElement2{
                    columnName: ""
                    columnText: "Density"
                }
                LeftTabSlicingOptionElement3{
                    id:option_support_density
                }

                Item{width:parent.width;height:2}//spacer

                LeftTabSlicingOptionElement2{
                    columnName: "Infill"
                    columnText: "Type"
                }
                LeftTabSlicingOptionElement{
                    id:option_infill
                    columnName: ""
                    columnContents: ["general","full","none"]
                }
                LeftTabSlicingOptionElement2{
                    columnName: ""
                    columnText: "Density"
                }
                LeftTabSlicingOptionElement3{
                    id:option_infill_density
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
            PropertyChanges { target: ltso; height : 28 }
        }
    ]


}
