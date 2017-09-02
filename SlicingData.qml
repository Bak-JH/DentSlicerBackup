import QtQuick 2.0

Item {
    width: 358
    height: 80

    Rectangle{
        anchors.fill: parent
        color: "white"
        //border.width: 1
        //border.color: "#E0E0E0"
        //radius: 1
    }

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

            font.family: "Arial"
            font.pixelSize: 11
            verticalAlignment: Text.AlignBottom
            color: "#686868"
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
            anchors.top : parent.top
            anchors.left: sdupleft.right
            anchors.topMargin: 3
            anchors.leftMargin: 8

            inputWidth: 100
            inputData: "145 layer"
            inputSource: "qrc:/resource/slicing_layer.png"
        }
        SlicingDataElement{
            anchors.bottom : parent.bottom
            anchors.left: sddownleft.right
            anchors.bottomMargin: 3
            anchors.leftMargin: 8

            inputWidth: 100
            inputData: "26 ml"
            inputSource: "qrc:/resource/slicing_volume.png"
        }
    }

}
