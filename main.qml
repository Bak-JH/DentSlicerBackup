import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Scene3D 2.0
import QtQuick.Window 2.2

import "glcode.js" as GLCode

ApplicationWindow {
    title: qsTr("DLPslicer")
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
        anchors.top: parent.top
        anchors.left: parent.left


    }
    LeftTab{
        id : lefttab
        anchors.top: uppertab.bottom
        anchors.left: parent.left
    }

    Scene3D {
        id: scene3d

        //anchors.fill: parent
        anchors.top : uppertab.bottom
        anchors.left : lefttab.right
        anchors.right : parent.right
        anchors.bottom : parent.bottom
        /*width: 600
        height: 400
        anchors.margins: 10*/
        focus: true
        aspects: ["input", "logic"]
        cameraAspectRatioMode: Scene3D.AutomaticAspectRatio

        MainView {}
    }

}
