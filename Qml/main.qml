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
				signal openFile(url path)

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
                        openFile(dragList[i])

                    dragList = []
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
			
			Item{
				id: featureMenuArea
				anchors.left: partList.right
				anchors.right: printsetting.left

				FeatureMenu {
					id: featureMenu
					height: 100
					anchors.top: parent.top
					anchors.topMargin: sidePadding
					anchors.horizontalCenter: parent.horizontalCenter
				}
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

			ViewMode {
				id: viewmode
				anchors.top: partList.bottom
				anchors.topMargin: sidePadding
				anchors.left: parent.left
				anchors.leftMargin: sidePadding
			}

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
				anchors.top: viewmode.bottom
				anchors.topMargin: sidePadding
				anchors.left: parent.left
				anchors.leftMargin: sidePadding
			}

			// CtrSpinBox {
			// 	id: dssffs
			// 	fromNum: -360
			// 	toNum: 360
			// 	label.text: "Y (deg)"
			// 	width: 200
			// 	height: 200
			// 	anchors.top: featureArea.bottom
			// 	anchors.topMargin: sidePadding
			// 	anchors.left: parent.left
			// 	anchors.leftMargin: sidePadding
			// }

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
		id : resumeWindow
		objectName: "resumeWindow"
	}
}

