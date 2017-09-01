import QtQuick 2.0

Rectangle{
    property string iconSource
    property string iconText
    width: buttonWidth
    height: buttonHeight

    anchors.top : parent.top
    anchors.bottom : tabgroupname.top

    color : "transparent"

    Image{anchors.margins:2; anchors.fill:parent; source:iconSource}
    Text{
        anchors.top:parent.top;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.topMargin: 16;
        text: iconText
        font.pixelSize: 16
        //font.bold: true
        font.family: "Arial"
        color: "#494949"
    }

}
