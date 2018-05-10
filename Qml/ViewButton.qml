import QtQuick 2.0

Item {
    property string inputSource

    width : 52
    height: 52

    Rectangle{
        id : icon
        anchors.fill: parent
        radius: 1

        //color:  "#FFFFFF"
        border.width: 1
        border.color: "#CCCCCC"

        Image {
            width: 35
            height: 35
            anchors.centerIn: parent
            source:inputSource

            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    icon.color = "#E3E3E5"
                    qm.setHandCursor();
                }
                onExited: {
                    icon.color = "#FFFFFF"
                    qm.resetCursor();
                }
            }
        }
    }


    states: [
        State{
            name:"none"
            PropertyChanges { target: icon; visible:false }
        }
    ]
}
