import QtQuick 2.0

Item {
    /*
    property alias mttab: mttab
    property alias mtz: mtz
    //property alias mtm: mtm
    property alias mtr: mtr
    property alias slicing_data:slicing_data

    //property bool isTab : true
    property bool isZoom : false
    property bool isMove : false
    property bool isRotate : false

    MeshTransformerTab{
        id:mttab
        objectName: "mttab"
        anchors.left : parent.left
        anchors.top : parent.top

        anchors.leftMargin: 400
        anchors.topMargin: 400

        visible: isTab
    }

    MeshTransformerZoom{
        id : mtz

        anchors.top: parent.top
        anchors.left: parent.left

        anchors.topMargin: 0
        anchors.leftMargin: 0
        visible: isZoom
    }

    MeshTransformerMove{
        id : mtm

        anchors.top: parent.top
        anchors.left: parent.left

        anchors.topMargin: 0
        anchors.leftMargin: 0
        visible: isMove
    }

    */

    UndoRedoButton{
        id: undoRedoButton
        objectName: "undoRedoButton"
        /*
        anchors.top : parent.top
        anchors.right : parent.right
        anchors.topMargin: 32
        anchors.rightMargin: 86
        */
        anchors.left: parent.left
        anchors.bottom : parent.bottom
        anchors.leftMargin: 22
        anchors.bottomMargin: 22
    }

    ViewChange{
        anchors.top : parent.top
        anchors.right : parent.right
        anchors.topMargin: 32
        anchors.rightMargin: 32
    }

    SlicingData{
        id : slicing_data
        objectName: "slicingData"
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        anchors.rightMargin: 22
        anchors.bottomMargin: 22
    }




}
