import QtQuick 2.0
import QtQuick.Layouts 1.3


Rectangle {
    width : 232
    height: 28
    radius : 2

    color:"#f5f5f5"
    //color : "red"

    RowLayout{
        width: 222
        anchors.left : parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 10
        spacing : 10

        Rectangle{
            width : 10
            height: 10
            radius: 5

            color: "#43becd"
        }

        Item{
            width : 170
            height: 28

            Text{
                width: 170
                height: 28

                font.family: "Helvetica"
                font.pointSize: 16
                verticalAlignment: Text.AlignVCenter
                text: "Cura.abc"

            }
        }

        Image{
            width : 13
            height: 16
            anchors.verticalCenter: parent.verticalCenter

            source: "qrc:/Resource/ic-delete.png"
        }

        Item { Layout.fillWidth: true }




    }





}
