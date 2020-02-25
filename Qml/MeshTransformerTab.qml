import QtQuick 2.0
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import Qt3D.Logic 2.0

Rectangle {
    width : 36
    height: 168

    property int leftPos: 0
    property int topPos: 0
    property int ftrDelete : 18

    anchors.leftMargin: leftPos
    anchors.topMargin: topPos

    visible: false
    color : "transparent"
    signal runGroupFeature(int type, string state, double arg1, double arg2, double arg3, var config);

    Rectangle{//shadow
        width: 34
        height: 34
        anchors.top : parent.top
        anchors.left: parent.left
        anchors.topMargin: 10
        anchors.leftMargin: 2

        color: "#CCCCCC"
    }
    Image {
        id: model_move
        width: 34
        height: 34

        anchors.top : parent.top
        anchors.left: parent.left
        anchors.topMargin: 8

        source: "qrc:/resource/model_move.png"
        MouseArea {
            anchors.fill: parent
            hoverEnabled : true
            onEntered : {
                model_move.source = "qrc:/resource/model_move_select.png"
                qm.setHandCursor();
            }
            onExited : {
                model_move.source = "qrc:/resource/model_move.png"
                qm.resetCursor();
            }
            onClicked: {
                deselectAll()
                selectMove()
            }
        }
    }

    Rectangle{//shadow
        width: 34
        height: 34
        anchors.top : model_move.bottom
        anchors.left: parent.left
        anchors.topMargin: 10
        anchors.leftMargin: 2

        color: "#CCCCCC"
    }
    Image {
        id: model_rotate
        width: 34
        height: 34

        anchors.top : model_move.bottom
        anchors.left: parent.left
        anchors.topMargin: 8

        source: "qrc:/resource/model_rotate.png"

        MouseArea {
            anchors.fill: parent
            hoverEnabled : true
            onEntered : {
                model_rotate.source = "qrc:/resource/model_rotate_select.png"
                qm.setHandCursor();
            }
            onExited : {
                model_rotate.source = "qrc:/resource/model_rotate.png"
                qm.resetCursor();
            }
            onClicked: {
                deselectAll()
                selectRotate()
            }
        }
    }

    Rectangle{//shadow
        width: 34
        height: 34
        anchors.top : model_rotate.bottom
        anchors.left: parent.left
        anchors.topMargin: 10
        anchors.leftMargin: 2

        color: "#CCCCCC"
    }
    Image {
        id: model_scale
        width: 34
        height: 34

        anchors.top : model_rotate.bottom
        anchors.left: parent.left
        anchors.topMargin: 8

        source: "qrc:/resource/model_scale.png"

        MouseArea {
            anchors.fill: parent
            hoverEnabled : true
            onEntered : {
                model_scale.source = "qrc:/resource/model_scale_select.png"
                qm.setHandCursor();
            }
            onExited : {
                model_scale.source = "qrc:/resource/model_scale.png"
                qm.resetCursor();
            }
            onClicked: {
                deselectAll()
                selectScale()
            }
        }
    }

    Rectangle{//shadow
        width: 34
        height: 34
        anchors.top : model_scale.bottom
        anchors.left: parent.left
        anchors.topMargin: 10
        anchors.leftMargin: 2
        color: "#CCCCCC"
    }

    Image {
        id: model_delete

        width: 34
        height: 34

        anchors.top : model_scale.bottom
        anchors.left: parent.left
        anchors.topMargin: 8

        source: "qrc:/resource/model_delete.png"

        MouseArea {
            anchors.fill: parent
            hoverEnabled : true
            onEntered : {
                model_delete.source = "qrc:/resource/model_delete_select.png"
                qm.setHandCursor();
            }
            onExited : {
                model_delete.source = "qrc:/resource/model_delete.png"
                qm.resetCursor();
            }
            onClicked: {
                deselectAll()
                selectDelete()
            }
        }
    }

    function setPosition(w, h){
        leftPos = w
        topPos = h
    }

    function perfectPosition(){
        var point = world2Screen(qm.getSelectedCenter())
        var boundedSizeHalf = qm.selectedModelsLengths().times(0.5)

        var target =[]
        var max = Qt.vector2d(0,0)
        var min = Qt.vector2d(window.width - lefttab.width, window.height - uppertab.height)

        target.push(world2Screen(qm.getSelectedCenter().plus(boundedSizeHalf.times(Qt.vector3d(1,1,1)))))
        target.push(world2Screen(qm.getSelectedCenter().plus(boundedSizeHalf.times(Qt.vector3d(1,1,-1)))))
        target.push(world2Screen(qm.getSelectedCenter().plus(boundedSizeHalf.times(Qt.vector3d(1,-1,1)))))
        target.push(world2Screen(qm.getSelectedCenter().plus(boundedSizeHalf.times(Qt.vector3d(1,-1,-1)))))
        target.push(world2Screen(qm.getSelectedCenter().plus(boundedSizeHalf.times(Qt.vector3d(-1,1,1)))))
        target.push(world2Screen(qm.getSelectedCenter().plus(boundedSizeHalf.times(Qt.vector3d(-1,1,-1)))))
        target.push(world2Screen(qm.getSelectedCenter().plus(boundedSizeHalf.times(Qt.vector3d(-1,-1,1)))))
        target.push(world2Screen(qm.getSelectedCenter().plus(boundedSizeHalf.times(Qt.vector3d(-1,-1,-1)))))

        for(var i=0; i<target.length; i++){
            if(max.x < target[i].x)
                max.x = target[i].x
            if(max.y < target[i].y)
                max.y = target[i].y
            if(min.x > target[i].x)
                min.x = target[i].x
            if(min.y > target[i].y)
                min.y = target[i].y
        }

        var targetLeftPos = parseInt(max.x) + 4
        var targetTopPos = parseInt(min.y)

        if(targetTopPos <= 0)
            targetTopPos = 10

        if(targetTopPos + 180 > window.height - uppertab.height)
            targetTopPos = window.height - uppertab.height - 180

        if(targetLeftPos + 40 > window.width - lefttab.width)
            targetLeftPos = parseInt(min.x) - 40


        setPosition(targetLeftPos,targetTopPos)
    }

    function deselectAll(){
        model_move.source = "qrc:/resource/model_move.png"
        model_rotate.source = "qrc:/resource/model_rotate.png"
        model_scale.source = "qrc:/resource/model_scale.png"
    }

    function selectMove(){
        model_move.source = "qrc:/resource/model_move_select.png"

        uppertab.all_off()
        uppertab.second_tab_button_move.state = "active"
        mttab.runGroupFeature(4, "active", 0, 0, 0, null);

        hideTab()
    }

    function selectRotate(){
        model_rotate.source = "qrc:/resource/model_rotate_select.png"

        uppertab.all_off()
        uppertab.second_tab_button_rotate.state = "active"
        mttab.runGroupFeature(5, "active", 0, 0, 0, null);

        hideTab()
    }

    function selectScale(){
        model_scale.source = "qrc:/resource/model_scale_select.png"

        uppertab.all_off()
        uppertab.third_tab_button_scale.state = "active"

        hideTab()
    }

    function selectDelete(){
        uppertab.all_off();
        yesnoPopUp.openYesNoPopUp(false, "", "Are you sure to delete these models?", "", 18, "", ftrDelete, 0)
        //deletePopUp.visible = true
        //deletePopUp.targetID = qm.getselectedModelID()

        hideTab()
    }

    function tabOnOff(){
        mttab.visible = !mttab.visible
        if(mttab.visible)
            perfectPosition()
    }

    function hideTab(){
        mttab.visible = false
    }

    function updatePosition(){
        if(mttab.visible)
            mttab.perfectPosition()
    }


}

