import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    width : 60
    height: 140
    Image{
        id : zoom
        source: "qrc:/resource/zoom.png"
        width : 60
        height : 60
        anchors.top : zoomUp.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 8
        //transform:Rotation{origin.x: arrow.width/2; origin.y: arrow.height/2; axis { x: 1; y: 0; z: 0 } angle: 0 }
        rotation: 0

        MouseArea {
            property vector2d pressPosition
            property vector2d prevPosition
            property vector2d currPosition
            property bool isDrag : false
            hoverEnabled: true
            anchors.fill: parent

            onPressed: {
                isDrag = true
                zoomColor.color = "#F2CA20"

                pressPosition = Qt.vector2d(mouseX , mouseY);
                prevPosition = Qt.vector2d(mouseX , mouseY);
                console.log("mouse  " + mouseX + "     " + mouseY)
            }
            onReleased: {
                isDrag = false
                zoomColor.color = "#606060"
                zoomUpColor.color = "#606060"
                zoomDownColor.color = "#606060"
            }
            onPositionChanged: {
                if(isDrag){
                    currPosition = Qt.vector2d(mouseX , mouseY);

                    if(pressPosition.y > currPosition.y){
                        zoom_Up();
                        zoomUpColor.color = "#F2CA20"
                        zoomDownColor.color = "#606060"
                    }else{
                        zoom_Down();
                        zoomUpColor.color = "#606060"
                        zoomDownColor.color = "#F2CA20"
                    }
                    prevPosition = currPosition
                }

            }
        }
    }

    Image{
        id : zoomUp
        source: "qrc:/resource/zoom_up.png"

        width : 32
        height : 32
        anchors.top : parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        //transform:Rotation{origin.x: arrow.width/2; origin.y: arrow.height/2; axis { x: 1; y: 0; z: 0 } angle: 0 }
    }

    Image{
        id : zoomDown
        source: "qrc:/resource/zoom_down.png"
        width : 32
        height : 32
        anchors.top : zoom.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 8

    }

    ColorOverlay{
        id : zoomColor
        anchors.fill: zoom
        source:zoom
        color : "#606060"
    }
    ColorOverlay{
        id : zoomUpColor
        anchors.fill: zoomUp
        source:zoomUp
        color : "#606060"
    }

    ColorOverlay{
        id : zoomDownColor
        anchors.fill: zoomDown
        source:zoomDown
        color : "#606060"
    }

    function show(w,h){
        var result = Qt.vector2d(0,0)

        result.x = w + scene3d.anchors.leftMargin
        result.y = h + scene3d.anchors.topMargin

        setPosition(result.x,result.y)
    }

    function setPosition(w, h){
        anchors.leftMargin = w
        anchors.topMargin = h
    }


    function zoom_Up(){
        console.log("zoom up")
        sceneRoot.total.meshEntity4.zoomUp()
        sceneRoot.total.meshEntity4.cut()
    }

    function zoom_Down(){
        console.log("zoom down")
        sceneRoot.total.meshEntity4.zoomDown()
    }
}
