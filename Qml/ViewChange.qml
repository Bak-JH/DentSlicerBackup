import QtQuick 2.4


import QtQuick.Controls 1.2
import QtQuick.Layouts 1.3


Item {
    width : 194
    height: 262

    Item{
        width: 50
        height: 52
        anchors.top : parent.top
        anchors.right : parent.right

        Rectangle{//shadow
            id : shadowRect
            width: parent.width-2
            height: parent.height-2
            anchors.bottom: parent.bottom
            anchors.right: parent.right

            color: "#CCCCCC"
        }

        Rectangle{//main
            id : contentRect
            width: parent.width-2
            height: parent.height-2
            anchors.top: parent.top
            anchors.left: parent.left

            color: "#ffffff"

            Image{
                anchors.fill: parent
                source:"qrc:/resource/view.png"
            }
            Text{
                text:"VIEW"
                anchors.top : parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: 6

                font.family: mainFont.name
                font.pixelSize: 12

            }
            MouseArea{
                anchors.fill: parent

                hoverEnabled : true
                onEntered : qm.setHandCursor();
                onExited : qm.resetCursor();

                onClicked: {
                    views.visible = !views.visible
                }
            }
        }

    }

    Image{
        id : views
        width : parent.width
        height : 200

        anchors.left: parent.left
        anchors.bottom: parent.bottom

        source:"qrc:/resource/view_background.png"

        visible: false;

        GridLayout{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10

            columns: 3
            rows : 3
            rowSpacing: 8
            columnSpacing: 8

            ViewButton{state:"none"}
            ViewButton{
                inputSource: "qrc:/resource/view_up.png"
                MouseArea{
                    anchors.fill: parent
                    onClicked: viewUp()
                }
            }
            ViewButton{
                inputSource: "qrc:/resource/view_back.png"
                MouseArea{
                    anchors.fill: parent
                    onClicked: viewBack()
                }
            }

            ViewButton{
                inputSource: "qrc:/resource/view_left.png"
                MouseArea{
                    anchors.fill: parent
                    onClicked: viewLeft()
                }
            }
            ViewButton{
                inputSource: "qrc:/resource/view_center.png"
                MouseArea{
                    anchors.fill: parent
                    onClicked: viewCenter()
                }
            }
            ViewButton{
                inputSource: "qrc:/resource/view_right.png"
                MouseArea{
                    anchors.fill: parent
                    onClicked: viewRight()
                }
            }

            ViewButton{
                inputSource: "qrc:/resource/view_front.png"
                MouseArea{
                    anchors.fill: parent
                    onClicked: viewFront()
                }
            }
            ViewButton{
                inputSource: "qrc:/resource/view_bottom.png"
                MouseArea{
                    anchors.fill: parent
                    onClicked: viewBottom()
                }
            }
            ViewButton{state:"none"}

        }

    }
    function viewUp(){
        sceneRoot.systemTransform.scale3D = Qt.vector3d(0.004,0.004,0.004)
        sceneRoot.systemTransform.rotationX = 0
        sceneRoot.systemTransform.rotationY = 0
        sceneRoot.systemTransform.rotationZ = 0

        sceneRoot.cm.camera.translateWorld(sceneRoot.cm.camera.viewCenter.times(-1))
        sceneRoot.cm.camera.translateWorld(Qt.vector3d(0.025,-0.25,0))

    }
    function viewBack(){
        sceneRoot.systemTransform.scale3D = Qt.vector3d(0.004,0.004,0.004)
        sceneRoot.systemTransform.rotationX = -90
        sceneRoot.systemTransform.rotationY = 0
        sceneRoot.systemTransform.rotationZ = -180

        sceneRoot.cm.camera.translateWorld(sceneRoot.cm.camera.viewCenter.times(-1))
        sceneRoot.cm.camera.translateWorld(Qt.vector3d(0.02,-0.06,0))
    }
    function viewLeft(){
        sceneRoot.systemTransform.scale3D = Qt.vector3d(0.004,0.004,0.004)
        sceneRoot.systemTransform.rotationX = -90
        sceneRoot.systemTransform.rotationY = 0
        sceneRoot.systemTransform.rotationZ = 90;

        sceneRoot.cm.camera.translateWorld(sceneRoot.cm.camera.viewCenter.times(-1))
        sceneRoot.cm.camera.translateWorld(Qt.vector3d(0.015,-0.04,0))
    }
    function viewRight(){
        sceneRoot.systemTransform.scale3D = Qt.vector3d(0.004,0.004,0.004)
        sceneRoot.systemTransform.rotationX = -90
        sceneRoot.systemTransform.rotationY = 0
        sceneRoot.systemTransform.rotationZ = -90;

        sceneRoot.cm.camera.translateWorld(sceneRoot.cm.camera.viewCenter.times(-1))
        sceneRoot.cm.camera.translateWorld(Qt.vector3d(0.015,-0.04,0))
    }
    function viewFront(){
        sceneRoot.systemTransform.scale3D = Qt.vector3d(0.004,0.004,0.004)
        sceneRoot.systemTransform.rotationX = -90
        sceneRoot.systemTransform.rotationY = 0
        sceneRoot.systemTransform.rotationZ = 0;

        sceneRoot.cm.camera.translateWorld(sceneRoot.cm.camera.viewCenter.times(-1))
        sceneRoot.cm.camera.translateWorld(Qt.vector3d(0.02,-0.06,0))
    }
    function viewBottom(){
        sceneRoot.systemTransform.scale3D = Qt.vector3d(0.004,0.004,0.004)
        sceneRoot.systemTransform.rotationX = -180
        sceneRoot.systemTransform.rotationY = 0
        sceneRoot.systemTransform.rotationZ = 0

        sceneRoot.cm.camera.translateWorld(sceneRoot.cm.camera.viewCenter.times(-1))
        sceneRoot.cm.camera.translateWorld(Qt.vector3d(0.025,-0.25,0))

    }
    function viewCenter(){
        sceneRoot.systemTransform.scale3D = Qt.vector3d(0.004,0.004,0.004)
        sceneRoot.systemTransform.rotationX = -70
        sceneRoot.systemTransform.rotationY = 0
        sceneRoot.systemTransform.rotationZ = -40

        sceneRoot.cm.camera.translateWorld(sceneRoot.cm.camera.viewCenter.times(-1))
        sceneRoot.cm.camera.translateWorld(Qt.vector3d(-0.015,-0.16,0))
    }


}
