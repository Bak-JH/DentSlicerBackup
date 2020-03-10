import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Scene3D 2.0
import QtQuick.Window 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import hix.qml 1.0 as Hix
import "Controls"
import "FeaturePopup"

Item{
	property var sidePadding: 20
    // ApplicationWindow {
    Hix.MainWindow {

        title: qsTr("DentSlicer")
        id : window
        width: 1280
        height: 768
        minimumWidth: 1500
        minimumHeight: 960
        visible: false

        objectName: "mainWindow"
        // property alias mttab : mttab
        property alias mainFont : mainFont
		property alias openRegular: openRegular
		property alias openSemiBold: openSemiBold
		property alias openBold: openBold


        FontLoader{ id : mainFont; source: "qrc:/Resource/font/NotoSans-SemiCondensed.ttf" }
        FontLoader{ id : mediumFont; source: "qrc:/Resource/font/NotoSans-SemiCondensedMedium.ttf" }
		FontLoader{ id: openRegular; source: "qrc:/Resource/font/OpenSans-Regular.ttf" }
		FontLoader{ id: openSemiBold; source: "qrc:/Resource/font/OpenSans-SemiBold.ttf" }
		FontLoader{ id: openBold; source: "qrc:/Resource/font/OpenSans-Bold.ttf" }
		
        Rectangle{

			id: uiRoot
            color: "#E5E5E5"
			anchors.fill: parent
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
            Scene3D {
                id: scene3d
                objectName: "scene3d"
                anchors.top : parent.top
                anchors.left : parent.left
                width: window.width
                height: window.height
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
			FeatureMenu {
				id: featureMenu
				height: 100
				anchors.top: parent.top
				anchors.left: partList.right
				anchors.leftMargin: 72
				anchors.topMargin: sidePadding
			}

			PartList{
				id: partList
				//width : 265
				//height : parent.height - featureMenu.height
				anchors.top: parent.top
				anchors.topMargin: sidePadding
				anchors.left: parent.left
				anchors.leftMargin: sidePadding
			}

			// ViewMode {
			// 	id: viewmode
			// 	anchors.top: partList.bottom
			// 	anchors.topMargin: sidePadding
			// 	anchors.left: parent.left
			// 	anchors.leftMargin: sidePadding
			// }

			UndoRedo {
				id: undoredo
				anchors.bottom: parent.bottom
				anchors.bottomMargin: 32
				anchors.horizontalCenter: partList.horizontalCenter
			}
			PrintSetting {
				id: printsetting
				anchors.top: parent.top
				anchors.topMargin: sidePadding
				anchors.right: parent.right
				anchors.rightMargin: sidePadding
			}

			Cam {
				id: cam
				anchors.top: printsetting.bottom
				anchors.topMargin: sidePadding * 1.5
				anchors.right: parent.right
				anchors.rightMargin: sidePadding + 15
			}

			Item {
				id: slideArea
				anchors.top: cam.bottom
				anchors.topMargin: 20
				anchors.left: cam.left
			}

			Item {
				id: featureArea
				objectName: "featureArea"
				anchors.top: partList.bottom
				anchors.topMargin: sidePadding
				anchors.left: parent.left
				anchors.leftMargin: sidePadding
			}

			// Toast {
			// 	id: toast
			// 	anchors.bottom: parent.bottom
			// 	anchors.bottomMargin: sidePadding
			// 	anchors.horizontalCenter: parent.horizontalCenter
			// }
			
			/*
			ExceptionToast {
				id: exceptiontoast
				anchors.bottom: parent.bottom
				anchors.bottomMargin: sidePadding
				anchors.horizontalCenter: parent.horizontalCenter
			}
			
			ModelBuildSlider {
				id: modelbuildslider
				anchors.top: cam.bottom
				anchors.topMargin: sidePadding * 1.5
				anchors.horizontalCenter: cam.horizontalCenter
			}
			
			SlideBar {
				id: slidebar
				anchors.top: cam.bottom
				anchors.topMargin: sidePadding * 1.5
				anchors.horizontalCenter: cam.horizontalCenter
			}
			*/
			
			PrintInfo {
				id: printInfo
				anchors.bottom: parent.bottom
				anchors.bottomMargin: sidePadding
				anchors.right: parent.right
				anchors.rightMargin: sidePadding
			}
			
			MouseArea{
				acceptedButtons: Qt.MiddleButton | Qt.RightButton
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
					// mttab.updatePosition()
				}

				onPositionChanged: {
					if(mode != 0)
					{
						currPosition = Qt.vector2d(mouseX,mouseY);

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
					// mouse wheel scaling: model and bed zooms, camera moves to mouse pointer direction
					var d = wheel.angleDelta.y;
					var scaleTmp = sceneRoot.systemTransform.scale3D;

					var v_c = sceneRoot.cm.camera.position.plus(Qt.vector3d(0.015, 0.16, -100));
					var v_m = {};                               // absolute position of mouse pointer
					var v_relative = Qt.vector3d(0,0,0);        // relative position of mouse pointer based on camera

					v_m.x = wheel.x;
					v_m.y = wheel.y;
					v_relative.x = (v_m.x / scene3d.width) - 0.5;
					v_relative.y = - ((v_m.y + ((scene3d.width - scene3d.height)/2)) / scene3d.width) + 0.5;

					v_relative.z = 0;

					if (d>0) {
						sceneRoot.systemTransform.scale3D = scaleTmp.times(1.08);
						v_c = v_c.plus(v_relative);
						sceneRoot.cm.camera.translateWorld(v_c.times(0.08));

						// mttab.updatePosition();
					}
					else {
						sceneRoot.systemTransform.scale3D = scaleTmp.times(0.92);
						v_c = v_c.plus(v_relative);
						sceneRoot.cm.camera.translateWorld(v_c.times(-0.08));
						// mttab.updatePosition();
					}
					sceneRoot.cameraViewChanged();
				}
			}

			Item 
			{
				id: dialogItem
			}




			// ResultPopup{
			// 	id : resultPopUp
			// }

			// ProgressPopup 
			// {
			// 	id: progressPopup
			// 	enabled: false
			// 	visible: false
			// }



			// PrintSettingPopup
			// {
			// 	id: printsettingpopup
			// 	anchors.top: featureMenu.bottom
			// 	anchors.left: leftpopupmove.right
			// 	anchors.leftMargin: 15
			// 	anchors.topMargin: 20
			// 	Component.onCompleted: {
			// 		printsettingpopup.setButtons([["Apply", "#00b9c8", "#21959e"], ["Cancel", "#abb3b3", "#8b9393"]]);
			// 	}

			// 	enabled: false
			// 	visible: false
			// }
			}

		// ResultPopup{
		// 	id : resultPopUp
		// }

		// ProgressPopup 
		// {
		// 	id: progressPopup
		// 	enabled: false
		// 	visible: false
		// }



		// PrintSettingPopup
		// {
		// 	id: printsettingpopup
		// 	anchors.top: featureMenu.bottom
		// 	anchors.left: leftpopupmove.right
		// 	anchors.leftMargin: 15
		// 	anchors.topMargin: 20
		// 	Component.onCompleted: {
		// 		printsettingpopup.setButtons([["Apply", "#00b9c8", "#21959e"], ["Cancel", "#abb3b3", "#8b9393"]]);
		// 	}
		// 	enabled: false
		// 	visible: false
		// }
		Item {
			id: popupArea
			anchors.horizontalCenter: window.horizontalCenter
			anchors.verticalCenter: window.verticalCenter
			objectName: "popupArea"
		}


    }

	Login{
		id : loginWindow
		objectName: "loginWindow"
	}

}

