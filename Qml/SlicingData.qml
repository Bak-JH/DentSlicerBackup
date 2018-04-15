import QtQuick 2.0

Item {
    id: slicing_data
    width: 360
    height: 82

    property alias slicing_layer:slicing_layer
    property alias slicing_volume:slicing_volume
    property alias slicing_time:sdupleft
    property alias slicing_size:sddownleft

    Rectangle{//shadow
        id : shadowRect
        width: parent.width-2
        height: parent.height-2
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        radius: 1

        color: "#CCCCCC"
    }

    Rectangle{//main
        id : contentRect
        width: parent.width-2
        height: parent.height-2
        anchors.top: parent.top
        anchors.left: parent.left

        radius: 1

        color: "#ffffff"

        Rectangle{
            id:icon
            width:68
            height: 60
            anchors.left : parent.left
            anchors.top : parent.top
            anchors.topMargin: 10

            color: "transparent"

            Image{
                width: 50
                height: 62
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                source: "qrc:/resource/slicing_refresh.png"
            }

            Text{
                text:"Refresh"
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter

                font.family: mainFont.name
                font.pixelSize: 11
                verticalAlignment: Text.AlignBottom
                color: "#FFFFFF"
            }


        }
        Rectangle{
            id:data
            width: 280
            height: 60
            anchors.left : icon.right
            anchors.top : parent.top
            anchors.topMargin: 10

            color: "transparent"

            border.width: 1
            border.color: "#E0E0E0"

            SlicingDataElement{
                id:sdupleft
                anchors.top : parent.top
                anchors.left: parent.left
                anchors.topMargin: 3
                anchors.leftMargin: 8

                inputWidth: 160
                inputData: "00h 20min"
                inputSource: "qrc:/resource/slicing_clock.png"
            }
            SlicingDataElement{
                id:sddownleft
                anchors.bottom : parent.bottom
                anchors.left: parent.left
                anchors.bottomMargin: 3
                anchors.leftMargin: 8

                inputWidth: 160
                inputData: "10.0 X 10.0 X 5.0 mm"
                inputSource: "qrc:/resource/slicing_size.png"
            }
            SlicingDataElement{
                id : slicing_layer
                anchors.top : parent.top
                anchors.left: sdupleft.right
                anchors.topMargin: 3
                anchors.leftMargin: 8

                inputWidth: 100
                inputData: "145 layer"
                inputSource: "qrc:/resource/slicing_layer.png"
            }
            SlicingDataElement{
                id: slicing_volume
                anchors.bottom : parent.bottom
                anchors.left: sddownleft.right
                anchors.bottomMargin: 3
                anchors.leftMargin: 8

                inputWidth: 100
                inputData: "26 ml"
                inputSource: "qrc:/resource/slicing_volume.png"
            }

            Connections {
                target: qm
                onUpdateModelInfo:{
                    slicing_time.inputData = parseInt(printing_time/60)+"h "+printing_time%60+"m";
                    slicing_layer.inputData = layer+" layer";

                    slicing_size.inputData = xyz;
                    slicing_volume.inputData = volume.toFixed(1)+ " ml";

                }
            }
        }

    }




}
