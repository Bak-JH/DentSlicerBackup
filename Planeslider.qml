import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.0
Slider {
    id:slider
    objectName: 'sslider'
    property string name:"slider"
    signal govalue(double value)
    maximumValue: 2
    minimumValue: -0.2
    visible: false
    orientation:Qt.Vertical
    updateValueWhileDragging: true
    tickmarksEnabled : true
    style: SliderStyle {
        groove: Image {
            source: "qrc:/resource/bar.png"
        }
        handle: Image {
            source:"qrc:/resource/switch.png"
        }
    }
    value:1
    onValueChanged: {
        if(value>1.8)
            value=1.8
        else if(value<0)
            value=0
        govalue(value)
}
}

