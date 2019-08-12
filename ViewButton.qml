import QtQuick 2.0

BlurRectangle {
    inputWidth: 90
    inputHeight: 90
    inputRadius : 45

    Text{
        width: parent.width
        height : 20

        anchors.top: parent.top
        anchors.topMargin: 18

        text : "VIEW"

        color: "#1db2c4"
        font.family: "Helvetica"
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: 16

    }

    Image {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 10
        source: "qrc:/Resource/view-triangle.png"
    }


}
