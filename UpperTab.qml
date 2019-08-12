import QtQuick 2.0
import QtQuick.Layouts 1.3

BlurRectangle {
    inputWidth : 1388
    inputHeight : 90
    inputRadius: 45

    inputColor : "#ffffff"

    RowLayout{
        id : layout
        anchors.left : parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 45
        spacing : 0

        UpperButton{
            inputSource : "ic-open"
            inputText: "Open"
        }

        Rectangle{
            width: 1
            height: 72
            anchors.verticalCenter: parent.verticalCenter
            color: "#DDDDDD"
        }

        UpperButton{
            inputSource : "ic-move"
            inputText: "Move"
        }
        UpperButton{
            inputSource : "ic-rotate"
            inputText: "Rotate"
        }
        UpperButton{
            inputSource : "ic-scale"
            inputText: "Scale"
        }

        Rectangle{
            width: 1
            height: 72
            anchors.verticalCenter: parent.verticalCenter
            color: "#DDDDDD"
        }

        UpperButton{
            inputSource : "ic-cut"
            inputText: "Cut"
        }
        UpperButton{
            width : 95
            inputSource : "ic-shelloffset"
            inputText: "Shell Offset"
        }
        UpperButton{
            inputSource : "ic-extend"
            inputText: "Extend"
        }
        UpperButton{
            inputSource : "ic-label"
            inputText: "Label"
        }
        UpperButton{
            inputSource : "ic-layflat"
            inputText: "Lay Flat"
        }
        UpperButton{
            inputSource : "ic-orient"
            inputText: "Orient"
        }
        UpperButton{
            inputSource : "ic-support"
            inputText: "Support"
        }
        UpperButton{
            inputSource : "ic-arrange"
            inputText: "Arrange"
        }
        UpperButton{
            width : 95
            inputSource : "ic-autorepair"
            inputText: "Auto Repair"
        }

        Rectangle{
            width: 1
            height: 72
            anchors.verticalCenter: parent.verticalCenter
            color: "#DDDDDD"
        }
        UpperButton{
            inputSource : "ic-save"
            inputText: "Save"
        }
        UpperButton{
            inputSource : "ic-export"
            inputText: "Export"
        }

    }

}
