import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.0

Slider {
    id:slider
    maximumValue: 2
    minimumValue: -0.2
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
    onValueChanged: {
        if(value>1.8)
            value=1.8
        else if(value<0)
            value=0
        sliderValueChanged(value);
    }
    signal sliderValueChanged(double value);
}

