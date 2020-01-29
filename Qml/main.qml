import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Scene3D 2.0
import QtQuick.Window 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import "Controls"
import "FeaturePopup"

Item{
    ApplicationWindow {
        title: qsTr("DentSlicer")
        id : window
        width: 1280
        height: 768
        minimumWidth: 1500
        minimumHeight: 960
        visible: false

        objectName: "mainWindow"
        property alias lefttabExport: lefttabExport
        property alias mttab : mttab

        property alias mainFont : mainFont

		property alias openRegular: openRegular
		property alias openSemiBold: openSemiBold
		property alias openBold: openBold

        FontLoader{
            id : mainFont
            source: "qrc:/Resource/font/NotoSans-SemiCondensed.ttf"

        }
        FontLoader{
            id : mediumFont
            source: "qrc:/Resource/font/NotoSans-SemiCondensedMedium.ttf"
        }

		FontLoader {
			id: openRegular
            source: "qrc:/Resource/font/OpenSans-Regular.ttf"
		}
		FontLoader {
			id: openSemiBold
            source: "qrc:/Resource/font/OpenSans-SemiBold.ttf"
		}
		FontLoader {
			id: openBold
            source: "qrc:/Resource/font/OpenSans-Bold.ttf"
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
                console.log(filePath)
                var filepath = filePath.toString().replace(/^(file:\/{3})/,"")
                console.log("opening" + filepath)

                if(filePath.split('.').pop().toLowerCase() === "stl") {
                    qm.openModelFile(filepath)
                    return filePath.split('.').pop().toLowerCase() === "stl"
                }
                else if(filePath.split('.').pop().toLowerCase() === "obj") {
                    qm.openModelFile(filepath)
                    return filePath.split('.').pop().toLowerCase() === "obj"
                }

            }
        }

        Rectangle{
            id : back
            anchors.top : featureMenu.bottom
            anchors.left : partlist.right
            anchors.right : parent.right
            anchors.bottom : parent.bottom
            color: "#E5E5E5"

            Scene3D {
                id: scene3d
                objectName: "scene3d"

                anchors.top : parent.top
                anchors.left : parent.left
                anchors.right : parent.right
                anchors.bottom : parent.bottom
                focus: true
                hoverEnabled: true
                aspects: ["input", "logic"]
                cameraAspectRatioMode: Scene3D.UserAspectRatio

                function disableScene3D(){
                    scene3d.enabled = false;
                }

                function enableScene3D(){
                    scene3d.enabled = true;
                }

                MainView {
                    objectName: "MainView"
                    id: sceneRoot
                }
            }
        }

		FeatureMenu {
			id: featureMenu
			height: 100
			anchors.top: parent.top
			anchors.left: partlist.right
			anchors.leftMargin: 15
			anchors.topMargin: 5
		}

		PartList{
			id: partlist
			anchors.top: featureMenu.bottom
            anchors.left: parent.left
			//z: 10
		}

        LeftTabExport{
            id : lefttabExport
            width : 264
            height : parent.height - featureMenu.height

            anchors.top: featureMenu.bottom
            anchors.left: partlist.right
            visible: false
        }

        MeshTransformerTab{
            id:mttab
            objectName: "mttab"
            anchors.left: partlist.right
            anchors.top : featureMenu.bottom
        }

        UICore{
            id:ui
            width: (window.width - partlist.width) * 1
            height: (window.height - featureMenu.height) * 1

            anchors.left: partlist.right
            anchors.top : featureMenu.bottom
            anchors.bottom: parent.bottom
        }
		


		PopupMove {
			id: leftpopupmove
			anchors.top: featureMenu.bottom
			anchors.left: partlist.right
			anchors.leftMargin: 15
			anchors.topMargin: 20
		}
		
		PopupLayFlat {
			id: leftpopuplayflat
			anchors.top: featureMenu.bottom
			anchors.left: leftpopupmove.right
			anchors.leftMargin: 15
			anchors.topMargin: 20
		}
		
		PopupLabel {
			id: leftpopuplabel
			anchors.top: featureMenu.bottom
			anchors.left: leftpopuplayflat.right
            anchors.leftMargin: 15
			anchors.topMargin: 20
		}
		/*
		FeatureMenu {
			id: bastardymenu
			anchors.top: uppertab.bottom
			anchors.left: lefttab.right
			anchors.leftMargin: 15
			anchors.topMargin: 20
		}
		*/

		
		/*
		FeaturePopup {
			id: leftpopup
			anchors.top: uppertab.bottom
			anchors.left: lefttab.right
			anchors.leftMargin: 15
			anchors.topMargin: 20
		}
		LeftPopupScale
		{
			id: leftpopupscale
			anchors.top: featureMenu.bottom
			anchors.left: leftpopuplabel.right
            anchors.leftMargin: 15
			anchors.topMargin: 20
		}

		LeftPopupMove {
			id: leftpopupmove
			anchors.top: uppertab.bottom
			anchors.left: lefttab.right
			anchors.leftMargin: 15
			anchors.topMargin: 20
		}*/
		PopupSupport
		{
			id: leftpopupsupport
			anchors.top: featureMenu.bottom
			anchors.left: partlist.right
            anchors.leftMargin: 15
			anchors.topMargin: 250
		}
		
		

		PopupCut {
			id: leftpopupcut
			anchors.top: leftpopupmove.bottom
			anchors.left: leftpopupmove.left
            anchors.leftMargin: 15
			anchors.topMargin: 15
		}
		PrintSetting
		{
			id: printsetting
			anchors.top: uppertab.bottom
			anchors.left: leftpopupmodelbuild.right
			anchors.leftMargin: 15
			anchors.topMargin: 250
		}
		
		
		Toast {
			id: toast
			anchors.bottom: window.bottom
			anchors.topMargin: 600
			anchors.left: window.left
			anchors.leftMargin: 15
		}
		
		UndoRedo {
			id: undoredo
			anchors.top: featureMenu.bottom
			anchors.topMargin: 720
			anchors.left: partlist.right
			anchors.leftMargin: 15
		}

		SlideBar {
			id: slidebar
			anchors.top: uppertab.bottom
			anchors.topMargin: 160
			anchors.left: leftpopuplabel.right
			anchors.leftMargin: 15
		}
		
		RangedSlider {
			id: buildslidebar
			anchors.top: uppertab.bottom
			anchors.topMargin: 160
			anchors.left: slidebar.right
			anchors.leftMargin: 15
		}

		Cam {
			id: cam
			anchors.top: featureMenu.bottom
			anchors.topMargin: 160
			anchors.left: slidebar.right
			anchors.leftMargin: 15
		}

		ViewMode {
			id: viewmode
			anchors.top: partlist.bottom
			anchors.left: window.left
			anchors.leftMargin: 15
			anchors.topMargin: 15
		}

		PrintInfo {
			id: printinfo
			anchors.top: slidebar.top
			anchors.topMargin: 500
			anchors.left: slidebar.right
			anchors.leftMargin: 100
		}
		
        MouseArea{
            acceptedButtons: Qt.MiddleButton | Qt.RightButton | Qt.LeftButton
            anchors.fill: parent
            property int mode: 0;// 0 = none, 1 = translate, 2 = rotate, !#!@# qt...
            property vector2d prevPosition;
            property vector2d currPosition;
            property real rotationSpeed : 0.2;
            propagateComposedEvents: true;

            onPressed: {
                if(mouse.button == Qt.RightButton)
                {
                    mode = 2;
                }
                else if(mouse.button == Qt.MiddleButton)
                {
                    mode = 1;
                }
                prevPosition = Qt.vector2d(mouseX,mouseY);
            }
            onReleased:  {
                mode = 0;
                mttab.updatePosition()
                qm.enableObjectPickers();
            }

            onPositionChanged: {
                if(mode != 0 && !qm.freecutActive)
                {
                    currPosition = Qt.vector2d(mouseX,mouseY);
                    qm.disableObjectPickers();

                    if(mode == 1){//mouse wheel drag
                        sceneRoot.cm.camera.translateWorld(Qt.vector3d((-1)*(currPosition.x - prevPosition.x)/1000,0,0),0);
                        sceneRoot.cm.camera.translateWorld(Qt.vector3d(0,(1)*(currPosition.y - prevPosition.y)/1000,0),0);
                    }
                    else
                    {
                        sceneRoot.systemTransform.rotationZ += rotationSpeed *(currPosition.x - prevPosition.x);
                        sceneRoot.systemTransform.rotationX += rotationSpeed *(currPosition.y - prevPosition.y);
                    }
                    prevPosition = currPosition;
                    sceneRoot.cameraViewChanged();

                }


            }

            onWheel: {
                qm.disableObjectPickers();
                // mouse wheel scaling: model and bed zooms, camera moves to mouse pointer direction
                var d = wheel.angleDelta.y;
                var scaleTmp = sceneRoot.systemTransform.scale3D;

                var v_c = sceneRoot.cm.camera.position.plus(Qt.vector3d(0.015, 0.16, -100));
                var v_m = {};                               // absolute position of mouse pointer
                var v_relative = Qt.vector3d(0,0,0);        // relative position of mouse pointer based on camera

                v_m.x = wheel.x - partlist.width;
                v_m.y = wheel.y - featureMenu.height;

                var height3d = (window.height - featureMenu.height)

                v_relative.x = (v_m.x / scene3d.width) - 0.5;
                v_relative.y = - ((v_m.y + ((scene3d.width - height3d)/2)) / scene3d.width) + 0.5;

                v_relative.z = 0;

                if (d>0) {
                    sceneRoot.systemTransform.scale3D = scaleTmp.times(1.08);
                    v_c = v_c.plus(v_relative);
                    sceneRoot.cm.camera.translateWorld(v_c.times(0.08));

                    mttab.updatePosition();
                }
                else {
                    sceneRoot.systemTransform.scale3D = scaleTmp.times(0.92);
                    v_c = v_c.plus(v_relative);
                    sceneRoot.cm.camera.translateWorld(v_c.times(-0.08));

                    mttab.updatePosition();
                }
                sceneRoot.cameraViewChanged();
                qm.enableObjectPickers();
            }
        }

        ResultPopup{
            id : resultPopUp
        }

		ProgressPopup 
		{
			id: progressPopup
			enabled: false
			visible: false
		}

		ModalWindow
		{
			id: modalWindow
			enabled: false
			visible: false
			
			Text {
				parent: modalWindow.contentArea
				text: modalWindow.modalMessage
				font.family: openRegular.name
				font.pointSize: 10
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter

			}
			
		}

		PrintSettingPopup
		{
			id: printsettingpopup
			anchors.top: featureMenu.bottom
			anchors.left: leftpopupmove.right
			anchors.leftMargin: 15
			anchors.topMargin: 20
			//enabled: false
			//visible: false
		}
    }

    Login{
        id : loginWindow
        objectName: "loginWindow"
    }
}

