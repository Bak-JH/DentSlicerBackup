import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Scene3D 2.0
import QtQuick.Window 2.2

import QtQuick.Controls.Styles 1.4
import "glcode.js" as GLCode
import QtQuick.Dialogs 1.2
ApplicationWindow {
    title: qsTr("DLPslicer")
    id : window
    width: 1280
    height: 768
    visible: false

    objectName: "mainWindow"
    property alias lefttab: lefttab

    menuBar: MenuBar{
        Menu{
            title : "File"
            MenuItem{text : "New File or Project"}
            MenuItem{text : "Open File of Project"}
            MenuItem{text : "Open File with ...."}
            MenuItem{text : "Recent Files"}
            MenuItem{text : "Recent Projects"}
            MenuItem{text : "Sessions"}
            MenuItem{text : "Session Menager...."}
            MenuItem{text : "Close Project \"  \""}
            MenuItem{text : "Print"}
            MenuItem{text : "Exit"}
        }
        Menu{
            title : "Edit"
            MenuItem{text : "help..."}
            MenuItem{text : "hellllp..."}
        }
    }
    Rectangle{
        id : back
        anchors.top : uppertab.bottom
        anchors.left : lefttab.right
        anchors.right : parent.right
        anchors.bottom : parent.bottom
//        color: "blue"
        color: "#EAEAEA"

    }

    /*
    Rectangle{
        anchors.top : uppertab.bottom
        anchors.left : lefttab.right
        width: (window.width - lefttab.width) * 1
        height: (window.height - uppertab.height) * 1

        //anchors.margins:10
        //anchors.leftMargin: - (window.width - lefttab.width)*2
        //anchors.topMargin: - (window.width - lefttab.width)*2
        MouseArea{
            anchors.fill: parent
            onClicked: {
                console.log("mouse x  " + mouseX)
            }
        }
        Scene3D {
            id: scene3d

            width: parent.width
            height: parent.height
            anchors.fill: parent

            focus: true
            hoverEnabled: true
            aspects: ["input", "logic"]
            cameraAspectRatioMode: Scene3D.AutomaticAspectRatio

            MainView {id: sceneRoot}
        }


    }*/

    Scene3D {
        id: scene3d
        /*
        width: parent.width
        height: parent.height
        anchors.fill: parent
        */
        anchors.top : uppertab.bottom
        anchors.left : lefttab.right
        width: (window.width - lefttab.width) * 1
        height: (window.width - lefttab.width) * 1
        //height: (window.height - uppertab.height) * 1
        //width: 800
        //height: 800

        focus: true
        hoverEnabled: true
        aspects: ["input", "logic"]
        cameraAspectRatioMode: Scene3D.AutomaticAspectRatio
        MainView {
            objectName: "MainView"
            id: sceneRoot
        }
    }



    UpperTab{
        id : uppertab
        width : parent.width
        height: 116

        function getCurrentText(){}
        options: [
             lefttab.ltso.option_resolution.currentText ,
             lefttab.ltso.option_layer_height.currentText,
             lefttab.ltso.option_support.currentText,
             lefttab.ltso.option_infill.currentText,
             lefttab.ltso.option_raft.currentText]

        anchors.top: parent.top
        anchors.left: parent.left

        //popup1.popup_anchor_left: lefttab.right
        //popup_anchor_top: lefttab.top

    }


    LeftTab{
        id : lefttab
        width : 260
        height : parent.height - uppertab.height


        anchors.top: uppertab.bottom
        anchors.left: parent.left
    }

    UICore{
        id:ui
        width: (window.width - lefttab.width) * 1
        height: (window.height - uppertab.height) * 1

        anchors.left: lefttab.right
        anchors.top : uppertab.bottom
        anchors.bottom: parent.bottom

    }

    FileDialogLoad{
        id: filedialogload
        visible: false
    }

    MouseArea{
        acceptedButtons: Qt.MiddleButton
        anchors.fill: parent
        property bool isDrag: false
        property vector2d prevPosition;
        property vector2d currPosition;

        onPressed: {
            isDrag = true;
            prevPosition = Qt.vector2d(mouseX,mouseY);
        }
        onReleased:  {
            isDrag = false
        }

        onPositionChanged: {
            if(isDrag){
                currPosition = Qt.vector2d(mouseX,mouseY);
                // move screen by drag scene3d

                scene3d.anchors.leftMargin = scene3d.anchors.leftMargin + (currPosition.x - prevPosition.x);
                scene3d.anchors.topMargin = scene3d.anchors.topMargin + (currPosition.y - prevPosition.y);

                /*
                var xDif = (currPosition.x - prevPosition.x);
                var yDif = (currPosition.y - prevPosition.y);

                sceneRoot.systemTransform.translation = sceneRoot.systemTransform.translation.plus(Qt.vector3d(0.001,0,0).times(xDif))
                sceneRoot.systemTransform.translation = sceneRoot.systemTransform.translation.plus(Qt.vector3d(0,-0.001,0).times(yDif))
                */
                prevPosition = currPosition;
            }
        }

    }
    PopUp {
        id: progress_popup
        funcname:""
        height: 70
        width: 300
        color: "#E5E5E5"
        border.color: "#D2D2D2"
        border.width: 1
        anchors.leftMargin: 0
        anchors.topMargin: 0
        detail1: ""
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        Text {
            id: progress_text1
            objectName: "progress_text"
            function update_loading(value) {
                progress_value.value=value;
                progress_text1.text=Math.round(value*100) + "% to complete";
                if(value==1){
                    progress_popup.state="inactive";
                    result_orient.state="active";
                    result_text.text="Orientation Complete.";
                }
            }
            text: "0% to complete"
            anchors.top: parent.top
            anchors.topMargin: 13
            anchors.horizontalCenter: parent.horizontalCenter
            font.pointSize: 7
            color: "#494949"
            font.family: "Arial"
        }
        Text {
            id: progress_text2
            text: "calculating..."
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            font.pointSize: 6
            color: "#999999"
            font.family: "Arial"
        }
        detailline1_vis: false
        detailline2_vis: false
        okbutton_vis: false
        applybutton_vis: false
        applyfinishbutton_vis: false
        descriptionimage_vis: false
        state: "inactive"
        ProgressBar {
            id:progress_value
            value: 0
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 13
            style:ProgressBarStyle {
                background: Rectangle{
                    color:"#EFEFEF"
                    implicitWidth: 265
                    implicitHeight: 5
                }
                progress: Rectangle{
                    color:"#44A6B6"
                    implicitHeight: 5
                    implicitWidth: progress_value.value
                }
            }
        }
    }
    PopUp {
        id: result_orient
        objectName: "result_orient"
        funcname: ""
        Text {
            id: result_text
            text:""
            anchors.top: parent.top
            anchors.topMargin: 30
            anchors.horizontalCenter: parent.horizontalCenter
            font.pointSize: 7
            color: "#494949"
            font.family: "Arial"
        }
        height: 100
        width: 230
        color: "#F2F2F2"
        anchors.leftMargin: 0
        anchors.topMargin: 0
        border.color: "#969696"
        border.width: 1
        detail1: ""
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        detailline1_vis: false
        detailline2_vis: false
        okbutton_vis: true
        applybutton_vis: false
        applyfinishbutton_vis: false
        descriptionimage_vis: true
        state: "inactive"

    }
    function moveH(value){
        scene3d.anchors.leftMargin = scene3d.anchors.leftMargin + parseInt(10*value)
    }

    function moveV(value){
        scene3d.anchors.topMargin = scene3d.anchors.topMargin + parseInt(-10*value)
    }

}
