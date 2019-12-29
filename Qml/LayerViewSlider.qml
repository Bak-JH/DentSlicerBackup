import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.0

Slider {
    id:slider
    objectName: "layerViewSlider"
    //property real modelHeight: modelHeight
    property int layerCount: layerCount
	property double layerThickness: 0

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
        rectValue.visible = pressed;
    }

    onValueChanged: {
        if(value > 9) {
            value = 9
        } else if(value < 0) {
            value = 0
        }

        var t = value / 9;
		var currIndex =  Math.round(t * layerCount);
        txtValue.text = currIndex.toString();
		sliderValueChanged(currIndex);

        // 0 ~ 9 -> 25 ~ 255
        rectValue.y = 255 - value * 230 / 9;
    }
    signal sliderValueChanged(int value);

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

    Rectangle{
        id : rectValue
        x: 0
        y: 15
        width: 30
        height: 20
        anchors.right: parent.left
        color: "#5ccbd0"
        anchors.rightMargin: -5
        visible: false

        Text{
            id : txtValue

            font.pixelSize: 12
            font.family: mainFont.name
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: "#ffffff"
            text: "0"
            anchors.fill: parent
        }

        Image{
            id : arrow
            x: 28
            source: "qrc:/resource/triangle.png"
            width : 6
            height : 10
            rotation: 90

            anchors.verticalCenter: parent.verticalCenter
            anchors.right : parent.right
            anchors.rightMargin: -6
        }
    }

    function setThickness(t){
        layerThickness = t;
    }

    function setLayerCount(v) {
        layerCount = v;
        txtMax.text = layerCount;
        slider.value = 9;
    }
	function getMaxLayer()
	{
		return layerCount;
	}
}

