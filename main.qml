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

    menuBar: MenuBar{
        Menu{
            title : "File"
            MenuItem{text : "Open..."}
            MenuItem{text : "Close..."}
        }
        Menu{
            title : "Help"
            MenuItem{text : "help..."}
            MenuItem{text : "hellllp..."}
        }
    }

    UpperTab{
        id : uppertab
        width : parent.width
        height: 116

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

    Rectangle{
        id : back
        anchors.top : uppertab.bottom
        anchors.left : lefttab.right
        anchors.right : parent.right
        anchors.bottom : parent.bottom
        color: "white"
    }



    Scene3D {
        id: scene3d

        //anchors.fill: parent
        anchors.top : uppertab.bottom
        anchors.left : lefttab.right
        //anchors.right : parent.right
        //anchors.bottom : parent.bottom
        width: 1200
        height: 800
        //anchors.margins: 10
        focus: true
        hoverEnabled: true
        aspects: ["input", "logic"]
        cameraAspectRatioMode: Scene3D.AutomaticAspectRatio

        MainView {id: sceneRoot}
    }

    /*
    MeshTransformerZoom{
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 100
        anchors.bottomMargin: 100
    }*/

    ViewChange{
        anchors.top : uppertab.bottom
        anchors.right : parent.right
        anchors.topMargin: 32
        anchors.rightMargin: 32
    }


    SlicingData{
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 22
        anchors.bottomMargin: 22
    }
    MeshTransformerTab{
        id:rt
        anchors.left : scene3d.left
        anchors.top:scene3d.top

        anchors.leftMargin: 400
        anchors.topMargin: 400
    }

    function moveH(){
        scene3d.anchors.leftMargin = scene3d.anchors.leftMargin + 1
    }

    function moveV(){
        scene3d.anchors.topMargin = scene3d.anchors.topMargin + 1
    }

}
