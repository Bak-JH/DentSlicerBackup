import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Scene3D 2.0
import QtQuick.Window 2.2
import QtCanvas3D 1.1
import QtQuick.Controls.Styles 1.4
import "glcode.js" as GLCode
import QtQuick.Dialogs 1.2
Item{
    ApplicationWindow {
        title: qsTr("DentStudio")
        id : window
        width: 1280
        height: 768
        minimumWidth: 1500
        minimumHeight: 960
        visible: false

        objectName: "mainWindow"
        property alias lefttab: lefttab
        property alias lefttabExport: lefttabExport
        property alias progressPopUp: progressPopUp
        property alias resultPopUp: resultPopUp
        property alias yesnoPopUp: yesnoPopUp
        property alias deletePopUp: deletePopUp
        property alias layerViewPopup: layerViewPopup
        property alias mttab : mttab

        property alias mainFont : mainFont

        FontLoader{
            id : mainFont
            source: "qrc:/Resource/font/NotoSans-SemiCondensed.ttf"
        }
        FontLoader{
            id : mediumFont
            source: "qrc:/Resource/font/NotoSans-SemiCondensedMedium.ttf"
        }

        DropArea {
            id: drop
            anchors.fill: parent
            property var dragList : []

            onEntered: {
                console.log("[Droparea] entered");
                for(var i = 0; i < drag.urls.length; i++)
                    dragList.push(drag.urls[i])
            }

            onExited: {
                dragList = []
                console.log("[Droparea] exited")
            }

            onDropped: {
                console.log("[Droparea] dropped")
                for(var i = 0; i < dragList.length; i++)
                    validateFileExtension(dragList[i])

                dragList = []
            }

            // Only STLs
            function validateFileExtension(filePath) {
                console.log(filePath);
                var filepath = filePath.toString().replace(/^(file:\/{3})/,"");
                console.log("opening" + filepath);

                if(filePath.split('.').pop().toLowerCase() === "stl")
                    qm.openModelFile(filepath);

                return filePath.split('.').pop().toLowerCase() === "stl"
            }
        }

        /* hidden for beta
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
        */
        Rectangle{
            id : back
            anchors.top : uppertab.bottom
            anchors.left : lefttab.right
            anchors.right : parent.right
            anchors.bottom : parent.bottom
            color: "#E5E5E5"
        }

        Scene3D {
            id: scene3d

            width: (window.width - lefttab.width) * 1
            height: (window.width - lefttab.width) * 1
            anchors.top : uppertab.bottom
            anchors.left : lefttab.right

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
            height: 100

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
            width : 265
            height : parent.height - uppertab.height


            anchors.top: uppertab.bottom
            anchors.left: parent.left
        }

        LeftTabExport{
            id : lefttabExport
            width : 264
            height : parent.height - uppertab.height

            anchors.top: uppertab.bottom
            anchors.left: parent.left
            visible: false
        }

        MeshTransformerTab{
            id:mttab
            objectName: "mttab"
            anchors.left: lefttab.right
            anchors.top : uppertab.bottom
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
                mttab.updatePosition()
            }

            onPositionChanged: {
                if(isDrag){//mouse wheel drag
                    currPosition = Qt.vector2d(mouseX,mouseY);
                    // move screen by drag scene3d
                    sceneRoot.cm.camera.translateWorld(Qt.vector3d((-1)*(currPosition.x - prevPosition.x)/1000,0,0),0);
                    sceneRoot.cm.camera.translateWorld(Qt.vector3d(0,(1)*(currPosition.y - prevPosition.y)/1000,0),0);
    //                scene3d.anchors.leftMargin = scene3d.anchors.leftMargin + (currPosition.x - prevPosition.x);
    //                scene3d.anchors.topMargin = scene3d.anchors.topMargin + (currPosition.y - prevPosition.y);
                    prevPosition = currPosition;
                }
            }
        }
        /*
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
                font.family: mainFont.name
            }
            Text {
                id: progress_text2
                text: "calculating..."
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                font.pointSize: 6
                color: "#999999"
                font.family: mainFont.name
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
        */
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
                font.pixelSize: 9
                color: "#494949"
                font.family: mainFont.name
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


        ArrangePopup{
            id : arrangePopUp
            objectName: "arrangePopup"

            visible: false
        }

        ProgressPopUp{
            id : progressPopUp
        }

        ResultPopup{
            id : resultPopUp
        }

        YesNoPopup{
            id : yesnoPopUp
        }

        DeletePopup{
            id : deletePopUp
        }

        LayerViewPopup {
            id: layerViewPopup
            objectName: "layerViewPopup"

            visible: false
        }

        LayerViewSlider {
            id: layerViewSlider
            objectName: "layerViewSlider"
            anchors.right: parent.right
            anchors.rightMargin:20
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Login{
        id : loginWindow
        objectName: "loginWindow"
    }
    /*
    ApplicationWindow {
        title: qsTr("login")
        id : loginWindow
        objectName: "loginWindow"
        width: 1280
        height: 768
        visible: false
        flags : Qt.FramelessWindowHint

        Rectangle{
            width: 400
            height: 400
            anchors.centerIn: parent
            color: "red"

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    loginWindow.close()
                    window.visible = true
                }
            }
        }
    }
    */
}

