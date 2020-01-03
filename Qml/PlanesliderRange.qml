import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Controls.Styles 1.0

RangeSlider {
    id:rangeSlider
    to: 2
    from: -0.2

    visible: false
    orientation:Qt.Vertical
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

    // background: Image {
    //     source: "qrc:/resource/bar.png"
    // }
    // first.handle: Image {
    //     source:"qrc:/resource/switch.png"
    // }
    // second.handle: Image {
    //     source:"qrc:/resource/switch.png"
    // }
    first.value: 1
    second.value: 1
    first.onMoved:
    {
        if(first.value>1.8)
            first.value=1.8
        else if(first.value<0)
            first.value=0
        sliderValueChangedFirst(first.value);
    }
    second.onMoved:
    {
        if(second.value>1.8)
            second.value=1.8
        else if(second.value<0)
            second.value=0
        sliderValueChangedSecond(second.value);
    }
    signal sliderValueChangedFirst(double value);
    signal sliderValueChangedSecond(double value);

}

