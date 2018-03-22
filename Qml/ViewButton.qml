import QtQuick 2.0

Item {
    property string inputSource

    width : 52
    height: 52

    Rectangle{
        id : icon
        anchors.fill: parent
        radius: 1

        color:  "#D6D6D6"
        border.width: 1
        border.color: "#CECECE"


        Image {
            width: 35
            height: 35
            anchors.centerIn: parent
            source:inputSource
        }
    }


    states: [
        State{
            name:"none"
            PropertyChanges { target: icon; visible:false }
        }
    ]
}
