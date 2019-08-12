import QtQuick 2.0

Rectangle {
    width: 85
    height: 88

    color: "#ffffff"

    property string inputText
    property string inputSource

    Text{
        width: parent.width
        height : 20

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 4

        text : inputText

        color: "#9d9d9d"
        font.family: "Helvetica"
        horizontalAlignment: Text.AlignHCenter

    }

    Image {
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        source: "qrc:/Resource/"+ inputSource +".png"
    }

}
