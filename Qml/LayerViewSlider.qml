import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.0

Slider {
    id:slider
    objectName: "layerViewSlider"
    property string name:"slider"
    maximumValue: 10
    minimumValue: -1
    value: 1

    visible: false
    orientation:Qt.Vertical
    updateValueWhileDragging: true
    tickmarksEnabled : true

    height:300

    Rectangle{//shadow
        width: parent.width
        height: 2
        anchors.left: parent.left
        anchors.top : parent.bottom
        anchors.leftMargin: 2
        color: "#CCCCCC"
    }
    Rectangle{//shadow
        width: 2
        height: parent.height
        anchors.left: parent.right
        anchors.top : parent.top
        anchors.topMargin: 2
        color: "#CCCCCC"
    }

    style: SliderStyle {
        groove: Image {
            source: "qrc:/resource/bar.png"
        }
        handle: Image {
            source:"qrc:/resource/switch.png"
        }
    }
    onPressedChanged: {
        txtValue.enabled = pressed;
    }

    onValueChanged: {
        if(value > 9)
            value = 9
        else if(value<0)
            value=0
        sliderValueChanged(value / 9);
        txtValue.text = value.toString();
        txtValue.y = height - height * value / 12 - 50;
    }
    signal sliderValueChanged(double value);

    Text{
        id : txtMin
        height: 20
        anchors.bottomMargin: 6
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        font.pixelSize: 14
        font.family: mainFont.name
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: "#000000"
        text: "0"
    }

    Text{
        id : txtMax
        height: 20
        anchors.topMargin: 6
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        font.pixelSize: 14
        font.family: mainFont.name
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: "#000000"
        text: "max"
    }

    Text{
        id : txtValue
        x: 15
        y: 15
        width: 30
        height: 20

        font.pixelSize: 16
        font.family: mainFont.name
        font.bold: true
        anchors.right: parent.left
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
        color: "#666666"
        text: "Color scheme"
    }
}

