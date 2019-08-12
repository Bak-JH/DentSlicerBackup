import QtQuick 2.0

Rectangle {
    color : "#F3F3F3"

    LeftTab{
        anchors.top : parent.top
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.topMargin: 20
    }

    UpperTab{
        anchors.top : parent.top
        anchors.left: parent.left
        anchors.leftMargin: 350
        anchors.topMargin: 20
    }

    ViewButton{
        anchors.top : parent.top
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.topMargin: 20
    }

    Rectangle{ //Undo Redo
        width: 180
        height: 60
        color: "transparent"
        anchors.bottom: parent.bottom
        anchors.left : parent.left
        anchors.bottomMargin: 56
        anchors.leftMargin: 56

        UndoRedoButton{
            anchors.top : parent.top
            anchors.left: parent.left
            inputSource: "qrc:/Resource/btn-undo.png"
        }
        UndoRedoButton{
            anchors.top : parent.top
            anchors.right: parent.right
            inputSource: "qrc:/Resource/btn-redo.png"
        }
    }

    SlicingInfo{
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.bottomMargin: 20
    }
    ViewModeTab{
        anchors.top : parent.top
        anchors.left: parent.left
        anchors.leftMargin: 287
        anchors.topMargin: 360
    }
    SlicingOptionTab{
        anchors.top : parent.top
        anchors.left: parent.left
        anchors.leftMargin: 288
        //anchors.topMargin: 430
        anchors.topMargin: 230
    }
}
