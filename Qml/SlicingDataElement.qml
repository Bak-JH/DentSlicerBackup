import QtQuick 2.0

Rectangle {
    property int inputWidth
    property string inputSource
    property string inputData
    width : inputWidth
    height: 26

    Image{
        id : icon
        width: 16
        height: 16
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter

        source: inputSource
        visible: true
    }

    Text{
        id :data

        anchors.left: icon.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 4

        text : inputData
        font.family: mainFont.name
        color: "#7C7C7C"
    }
}
