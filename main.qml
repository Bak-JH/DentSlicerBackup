import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Scene3D 2.0
import QtQuick.Window 2.2

import "glcode.js" as GLCode
import QtQuick.Dialogs 1.2
ApplicationWindow {
    title: qsTr("DLPslicer")
    id : window
    width: 1280
    height: 768
    visible: true

    property alias uppertab: uppertab
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
        //color: "blue"
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

    function moveH(value){
        scene3d.anchors.leftMargin = scene3d.anchors.leftMargin + parseInt(10*value)
    }

    function moveV(value){
        scene3d.anchors.topMargin = scene3d.anchors.topMargin + parseInt(-10*value)
    }

}
