import QtQuick 2.0

Item {
    Image{
        id : zoom
        source: "qrc:/resource/zoom.png"
        width : 60
        height : 60
        //transform:Rotation{origin.x: arrow.width/2; origin.y: arrow.height/2; axis { x: 1; y: 0; z: 0 } angle: 0 }
        rotation: 0

        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter


        MouseArea {
                anchors.fill: parent
                onClicked: { ltvm.state == 'open' ? ltvm.state = "close" : ltvm.state = 'open';}
        }
    }
}
