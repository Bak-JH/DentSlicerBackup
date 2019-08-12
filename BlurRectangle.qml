import QtQuick 2.0
import QtGraphicalEffects 1.0

Rectangle {
    property int inputWidth
    property int inputHeight
    property int inputRadius
    property string inputColor : "#ffffff"

    width : inputWidth
    height : inputHeight
    radius : inputRadius


    RectangularGlow {
        anchors.fill: parent
        glowRadius: 3
        spread: 0.6

        color: "#000000"
        opacity: 0.2

        cornerRadius: inputRadius + 2
    }

    Rectangle{
        anchors.fill: parent
        radius: inputRadius
        color : inputColor
    }



}
