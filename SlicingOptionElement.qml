import QtQuick 2.0
import QtQuick.Layouts 1.3

Item {
    property string inputTitle
    width: 240
    height: 28

    RowLayout{
        anchors.fill: parent
        spacing: 16

        Item {
            width : 110
            height: parent.height

            Text{
                width: parent.width
                height: parent.height

                font.family: "Helvetica"
                font.pointSize: 14
                verticalAlignment: Text.AlignVCenter
                text: inputTitle

            }
        }

        Rectangle{
            width : 114
            height: parent.height
        }
    }





}
