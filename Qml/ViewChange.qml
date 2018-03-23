import QtQuick 2.0


import QtQuick.Controls 1.2
import QtQuick.Layouts 1.3


Item {
    width : 194
    height: 262

    Item{
        width: 48
        height: 50
        anchors.top : parent.top
        anchors.right : parent.right

        //color: "red"
        Image{
            anchors.fill: parent
            source:"qrc:/resource/view.png"
        }
        Text{
            text:"VIEW"
            anchors.top : parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 6

            font.family: "Arial"
            font.pixelSize: 12

        }
        MouseArea{
            anchors.fill: parent
            onClicked: {
                views.visible = !views.visible
            }
        }
    }
    Image{
        id : views
        width : parent.width
        height : 200

        anchors.left: parent.left
        anchors.bottom: parent.bottom

        source:"qrc:/resource/view_background.png"

        visible: false;

        GridLayout{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10

            columns: 3
            rows : 3
            rowSpacing: 8
            columnSpacing: 8

            ViewButton{state:"none"}
            ViewButton{inputSource: "qrc:/resource/view_up.png"}
            ViewButton{inputSource: "qrc:/resource/view_back.png"}

            ViewButton{inputSource: "qrc:/resource/view_left.png"}
            ViewButton{inputSource: "qrc:/resource/view_center.png"}
            ViewButton{inputSource: "qrc:/resource/view_right.png"}

            ViewButton{inputSource: "qrc:/resource/view_front.png"}
            ViewButton{inputSource: "qrc:/resource/view_bottom.png"}
            ViewButton{state:"none"}

        }

    }


}
