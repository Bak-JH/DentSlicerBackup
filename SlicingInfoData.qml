import QtQuick 2.0
import QtQuick.Layouts 1.3

Rectangle {
    width: 120
    height: 20

    property string inputSource

    RowLayout{
        anchors.left : parent.left

        spacing : 8

        Image{
            width: 20
            height: 20
            source: inputSource
        }

        Text {
            width: 100
            height: 20
            font.family: "Helvetica"
            font.pointSize: 12
            verticalAlignment: Text.AlignVCenter

            text: "100*200*300 mm"
        }
    }




}
