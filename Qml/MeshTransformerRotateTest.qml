import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    width : 200
    height: 200

    property string xColor: "#000000"
    property string yColor: "#000000"
    property string zColor: "#000000"
    property int axisClicked : 0;

    Image{
        id : rotateBack
        source: "qrc:/resource/rotate_back.png"
        width : 194
        height : 194
        anchors.verticalCenter:  parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

    }
    Image{
        id : rotateX
        source: "qrc:/resource/rotate_x.png"
        width : 200
        height : 200
        anchors.verticalCenter:  parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        MouseArea {
            property vector2d pressPosition
            property vector2d prevPosition
            property vector2d currPosition
            property bool isDrag : false
            anchors.fill: parent
            hoverEnabled: true

            onEntered: {
                xColor = "#F2CA20"
            }

            onExited: {
                xColor = "#606060"
            }
            onPressed: {
                moveY.visible = false
                moveYColor.visible = false
                moveY2.visible = false
                moveY2Color.visible = false
                moveZ.visible = false
                moveZColor.visible = false

                axisClicked = 1;
            }

            onReleased: {
                moveY.visible = true
                moveYColor.visible = true
                moveY2.visible = true
                moveY2Color.visible = true
                moveZ.visible = true
                moveZColor.visible = true
                axisClicked = 0;
            }
        }
    }
    Image{
        id : rotateX2
        source: "qrc:/resource/rotate_x2.png"
        width : 200
        height : 200
        anchors.verticalCenter:  parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        MouseArea {
            property vector2d pressPosition
            property vector2d prevPosition
            property vector2d currPosition
            property bool isDrag : false
            anchors.fill: parent
            hoverEnabled: true

            onEntered: {
                xColor = "#F2CA20"
            }

            onExited: {
                xColor = "#606060"
            }
            onPressed: {
                moveY.visible = false
                moveYColor.visible = false
                moveY2.visible = false
                moveY2Color.visible = false
                moveZ.visible = false
                moveZColor.visible = false

                axisClicked = 1;
            }

            onReleased: {
                moveY.visible = true
                moveYColor.visible = true
                moveY2.visible = true
                moveY2Color.visible = true
                moveZ.visible = true
                moveZColor.visible = true
                axisClicked = 0;
            }
        }
    }
    Image{
        id : rotateY
        source: "qrc:/resource/rotate_y.png"
        width : 200
        height : 200
        anchors.verticalCenter:  parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        MouseArea {
            property vector2d pressPosition
            property vector2d prevPosition
            property vector2d currPosition
            property bool isDrag : false
            anchors.fill: parent
            hoverEnabled: true

            onEntered: {
                yColor = "#F2CA20"
            }

            onExited: {
                yColor = "#606060"
            }
            onPressed: {
                moveX.visible = false
                moveXColor.visible = false
                moveX2.visible = false
                moveX2Color.visible = false
                moveZ.visible = false
                moveZColor.visible = false

                axisClicked = 2;
            }

            onReleased: {
                moveX.visible = true
                moveXColor.visible = true
                moveX2.visible = true
                moveX2Color.visible = true
                moveZ.visible = true
                moveZColor.visible = true
                axisClicked = 0;
            }
        }
    }
    Image{
        id : rotateY2
        source: "qrc:/resource/rotate_y2.png"
        width : 200
        height : 200
        anchors.verticalCenter:  parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        MouseArea {
            property vector2d pressPosition
            property vector2d prevPosition
            property vector2d currPosition
            property bool isDrag : false
            anchors.fill: parent
            hoverEnabled: true

            onEntered: {
                yColor = "#F2CA20"
            }

            onExited: {
                yColor = "#606060"
            }
            onPressed: {
                moveX.visible = false
                moveXColor.visible = false
                moveX2.visible = false
                moveX2Color.visible = false
                moveZ.visible = false
                moveZColor.visible = false

                axisClicked = 2;
            }

            onReleased: {
                moveX.visible = true
                moveXColor.visible = true
                moveX2.visible = true
                moveX2Color.visible = true
                moveZ.visible = true
                moveZColor.visible = true
                axisClicked = 0;
            }
        }
    }

    Image{
        id : rotateZ
        source: "qrc:/resource/rotate_z.png"
        width : 200
        height : 200
        anchors.verticalCenter:  parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        MouseArea {
            property vector2d pressPosition
            property vector2d prevPosition
            property vector2d currPosition
            property bool isDrag : false
            anchors.fill: parent
            hoverEnabled: true

            onEntered: {
                zColor = "#F2CA20"
            }

            onExited: {
                zColor = "#606060"
            }
            onPressed: {
                rotateX.visible = false
                rotateXColor.visible = false
                rotateX2.visible = false
                rotateX2Color.visible = false
                rotateY.visible = false
                rotateYColor.visible = false
                rotateY2.visible = false
                rotateY2Color.visible = false

                axisClicked = 3;
            }

            onReleased: {
                rotateX.visible = true
                rotateXColor.visible = true
                rotateX2.visible = true
                rotateX2Color.visible = true
                rotateY.visible = true
                rotateYColor.visible = true
                rotateY2.visible = true
                rotateY2Color.visible = true

                axisClicked = 0;
            }
        }
    }


    ColorOverlay{
        id : rotateXColor
        anchors.fill: rotateX
        source:rotateX

        color : xColor

    }
    ColorOverlay{
        id : rotateX2Color
        anchors.fill: rotateX2
        source:rotateX2

        color : xColor

    }
    ColorOverlay{
        id : rotateYColor
        anchors.fill: rotateY
        source:rotateY

        color : yColor

    }
    ColorOverlay{
        id : rotateY2Color
        anchors.fill: rotateY2
        source:rotateY2

        color : yColor

    }

    ColorOverlay{
        id : rotateZColor
        anchors.fill: rotateZ
        source:rotateZ

        color : zColor

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

}
