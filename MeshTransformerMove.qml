import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    width : 200
    height: 200
    property int axisClicked : 0;

    Image{
        id : moveCenter
        source: "qrc:/resource/move_circle.png"
        width : 8
        height : 8
        anchors.verticalCenter:  parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

    }
    Image{
        id : moveX
        source: "qrc:/resource/move_arrow.png"
        width : 200
        height : 10
        anchors.verticalCenter:  parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        //rotation: 30

        MouseArea {
            property vector2d pressPosition
            property vector2d prevPosition
            property vector2d currPosition
            property bool isDrag : false
            anchors.fill: parent
            hoverEnabled: true

            onEntered: {
                moveXColor.color = "#F2CA20"
            }

            onExited: {
                moveXColor.color = "#606060"
            }

            onPressed: {
                isDrag = true
                moveY.visible = false
                moveYColor.visible = false
                axisClicked = 1;
                pressPosition = Qt.vector2d(mouseX , mouseY);
                prevPosition = Qt.vector2d(mouseX , mouseY);
            }

            onReleased: {
                isDrag = false
                moveY.visible = true
                moveYColor.visible = true
                axisClicked = 0;
            }
        }
    }

    Image{
        id : moveY
        source: "qrc:/resource/move_arrow.png"
        width : 200
        height : 10
        anchors.verticalCenter:  parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        rotation: 90

        MouseArea {
            property vector2d pressPosition
            property vector2d prevPosition
            property vector2d currPosition
            property bool isDrag : false
            anchors.fill: parent
            hoverEnabled: true

            onEntered: {
                moveYColor.color = "#F2CA20"
            }

            onExited: {
                moveYColor.color = "#606060"
            }
            onPressed: {
                isDrag = true
                moveX.visible = false
                moveXColor.visible = false
                axisClicked = 2;
                pressPosition = Qt.vector2d(mouseX , mouseY);
                prevPosition = Qt.vector2d(mouseX , mouseY);
            }

            onReleased: {
                isDrag = false
                moveX.visible = true
                moveXColor.visible = true
                axisClicked = 0;
            }
            onPositionChanged: {
                if(isDrag){
                    currPosition = Qt.vector2d(mouseX , mouseY);

                    if(pressPosition.y > currPosition.y){
                        moveYPlus();
                    }else{
                        moveYMinus();
                    }
                    prevPosition = currPosition

                }
            }
        }
    }


    ColorOverlay{
        id : moveXColor
        anchors.fill: moveX
        source:moveX
        color : "#606060"
        //rotation: 30
    }
    ColorOverlay{
        id : moveYColor
        anchors.fill: moveY
        source:moveY

        color : "#606060"
        rotation: 90

    }
    ColorOverlay{
        id : centerColor
        anchors.fill: moveCenter
        source:moveCenter
        color : "#191919"
    }

    function show(w,h){
        ui.world2Screen()

        var result = Qt.vector2d(0,0)

        result.x = w + scene3d.anchors.leftMargin
        result.y = h + scene3d.anchors.topMargin


        setPosition(result.x,result.y)
    }

    function setPosition(w, h){
        anchors.leftMargin = w
        anchors.topMargin = h
    }

    /*
    function moveXPlus(){
        meshTransform.translation = meshTransform.translation.minus(Qt.vector3d(0.01,0.00,0.00))
    }
    function moveXMinus(){
        meshTransform.translation = meshTransform.translation.minus(Qt.vector3d(0.01,0.00,0.00))
    }*/

    function moveYPlus(){

        console.log("y plus")
        sceneRoot.total.meshEntity4.moveYPlus()
    }
    function moveYMinus(){
        console.log("y minus")
        sceneRoot.total.meshEntity4.moveYMinus()
    }
}
