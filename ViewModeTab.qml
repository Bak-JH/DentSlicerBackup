import QtQuick 2.0
import QtQuick.Layouts 1.3

Rectangle{
    width: 185
    height: 122
    color: "transparent"

    Image{
        width: parent.width
        height: parent.height
        source: "qrc:/Resource/tab-viewmode.png"
    }

    ColumnLayout{  // Update to ScrollView
        width: 164
        height: 88
        anchors.verticalCenter: parent.verticalCenter
        anchors.right : parent.right
        anchors.rightMargin: 6
        spacing : 8

        Rectangle{
            width: parent.width
            height: 24
            color: "transparent"

            Text{
                width: parent.width
                height : 20

                text : "Object View"

                color: "#000000"
                font.family: "Helvetica"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pointSize: 16
            }
        }

        Rectangle{
            width: parent.width
            height: 24
            color: "transparent"

            Text{
                width: parent.width
                height : 20

                text : "Support View"

                color: "#000000"
                font.family: "Helvetica"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pointSize: 16
            }
        }

        Rectangle{
            width: parent.width
            height: 24
            color: "transparent"

            Text{
                width: parent.width
                height : 20

                text : "Layer View"

                color: "#000000"
                font.family: "Helvetica"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pointSize: 16
            }
        }


    }





}
