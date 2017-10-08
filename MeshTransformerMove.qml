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

        rotation: 30

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
                moveY.visible = false
                moveYColor.visible = false
                axisClicked = 1;
            }

            onReleased: {
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

        rotation: 110

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
                moveX.visible = false
                moveXColor.visible = false
                axisClicked = 2;
            }

            onReleased: {
                moveX.visible = true
                moveXColor.visible = true
                axisClicked = 0;
            }
        }
    }


    ColorOverlay{
        id : moveXColor
        anchors.fill: moveX
        source:moveX
        color : "#606060"
        rotation: 30
    }
    ColorOverlay{
        id : moveYColor
        anchors.fill: moveY
        source:moveY

        color : "#606060"
        rotation: 110

    }
    ColorOverlay{
        id : centerColor
        anchors.fill: moveCenter
        source:moveCenter
        color : "#191919"
    }

    function perfectPosition(w, h){
        mtz.anchors.topMargin = w;
        mtz.anchors.leftMargin = h;
    }
}
