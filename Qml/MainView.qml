
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import QtQuick 2.7
//import GLQML 1.0


import Qt3D.Logic 2.0

Entity {
    property vector3d xup : Qt.vector3d( 1.0, 0.0, 0.0 )
    property vector3d xdown : Qt.vector3d( -1.0, 0.0, 0.0 )

    property vector3d yup : Qt.vector3d( 0.0, 1.0, 0.0 )
    property vector3d ydown : Qt.vector3d( 0.0, -1.0, 0.0 )
    property vector3d zup : Qt.vector3d( 0.0, 0.0, 1.0 )
    property vector3d zdown : Qt.vector3d( 0.0, 0.0, -1.0 )

    property alias total: total
    property alias cm: cm
    property alias systemTransform: systemTransform

    //CoordinateMesh{} // 기준좌표 체크

    CameraManager{id : cm}

    MouseHandler{
        id : mouseHandler
        sourceDevice: mouse

        onPressed: {
            //console.log(mouse.X + " " + mouse.Y)
        }

        onWheel: {
            var Camera_position, average, temp, Camera_radius
            var d = wheel.angleDelta.y ;
            /*
            if(d>0)
                cm.zoomUp()
            else if(d<0)
                cm.zoomDown()
            */
            var scaleTmp = systemTransform.scale3D;
            if(d>0){
                systemTransform.scale3D = scaleTmp.times(1.08);
                //console.log("Dfdf")
                //console.log(systemTransform.scale3D)
                //sceneRoot.systemTransform.scale3D = sceneRoot.systemTransform.scale3D.plus(Qt.vector3d(0.02,0.02,0.02))
                //systemTransform.scale3D = systemTransform.scale3D.plus(Qt.vector3d(0.0002,0.0002,0.0002))
                //console.log(systemTransform.scale3D)
            }
            else if(d<0){
                systemTransform.scale3D = scaleTmp.times(0.92);
                //sceneRoot.systemTransform.scale3D = sceneRoot.systemTransform.scale3D.minus(Qt.vector3d(0.02,0.02,0.02))
                //systemTransform.scale3D = systemTransform.scale3D.minus(Qt.vector3d(0.0002,0.0002,0.0002))
            }

        }
    }

    Entity{
        id : total
        objectName:"total"
        Transform{
            id: systemTransform
            scale3D: Qt.vector3d(0.01,0.01,0.01)
        }
        components: [systemTransform]

        Plate{
            id: planeEntity
        }

        Model{
            id: meshEntity4
            //inputSource:"file:///D:/Dev/DLPSlicer/DLPslicer/resource/mesh/lowerjaw.obj"
        }
        Entity{
            objectName: "rotateSphereEntity"
            MeshTransformerRotate{
                objectName: "rotateSphere"
            }
        }
        Entity{
            objectName: "moveArrowEntity"
            MeshTransformerMove{
                objectName: "moveArrow"
            }
        }
        CoordinateMesh{}
    }

    MouseDevice {
        id: mouse
        sensitivity: 0.005
    }

    components: [
        LogicalDevice {
              id: logicalDevice;

              actions: [
                  Action {
                      id: rotateAction
                      inputs: [
                          ActionInput {
                              sourceDevice: mouse
                              buttons: [ MouseEvent.RightButton ]
                          }
                      ]
                  }
              ]


              axes: [
                  Axis {
                      id: rotateXAxis
                      inputs: [
                          AnalogAxisInput {
                              sourceDevice: mouse
                              axis: MouseDevice.X
                          }
                      ]
                  },
                  Axis {
                      id: rotateYAxis
                      inputs: [
                          AnalogAxisInput {
                              sourceDevice: mouse
                              axis: MouseDevice.Y
                          }

                      ]
                  }
              ]
          },

        FrameAction {
            property real rotationSpeed : 20

            onTriggered: {
                if (rotateAction.active) {
                    var target = axisAngle2Quaternion(rotationSpeed * rotateXAxis.value * dt,qq.rotatedVector(systemTransform.rotation,zdown))
                    cm.camera.rotateAboutViewCenter(qq.multiplyQuaternion(target,systemTransform.rotation));
                }
            }
        }

    ]

    KeyboardDevice{
        id : keyboardDevice
    }
    KeyboardHandler{
        focus : true
        id : keyboardHandler
        sourceDevice: keyboardDevice

        onPressed: {

            if (event.key === Qt.Key_Z) {
                meshTransform.scale3D = meshTransform.scale3D.plus(Qt.vector3d(0.05,0.05,0.05));
            }
            if (event.key === Qt.Key_X) {
                meshTransform.scale3D = meshTransform.scale3D.minus(Qt.vector3d(0.05,0.05,0.05));
            }

            if (event.key === Qt.Key_J) {
                systemTransform.translation = systemTransform.translation.plus(Qt.vector3d(-0.01,0,0))
            }
            if (event.key === Qt.Key_K) {
                systemTransform.translation = systemTransform.translation.plus(Qt.vector3d(0,-0.01,0))
            }
            if (event.key === Qt.Key_I) {
                console.log("i");
                systemTransform.translation = systemTransform.translation.plus(Qt.vector3d(0,0.01,0))
            }
            if (event.key === Qt.Key_L) {
                systemTransform.translation = systemTransform.translation.plus(Qt.vector3d(0.01,0,0))
            }
            /*
            if (event.key === Qt.Key_W) {
                scene3d.anchors.topMargin = scene3d.anchors.topMargin - 1
            }
            if (event.key === Qt.Key_A) {
                scene3d.anchors.leftMargin = scene3d.anchors.leftMargin - 1
            }
            if (event.key === Qt.Key_S) {
                scene3d.anchors.topMargin = scene3d.anchors.topMargin + 1
            }
            if (event.key === Qt.Key_D) {
                scene3d.anchors.leftMargin = scene3d.anchors.leftMargin + 1
            }*/

            if (event.key === Qt.Key_Q) {
                scene3d.width = scene3d.width -2
                scene3d.height = scene3d.height -1
            }
            if (event.key === Qt.Key_E) {
                scene3d.width = scene3d.width +2
                scene3d.height = scene3d.height +1

            }
            if (event.key === Qt.Key_M) {
                //loadOBJFile()
                //var newObject = Qt.createQmlObject('import QtQuick 2.0; Model{inputSource:"file:///D:/Dev/DLPSlicer/DLPslicer/resource/mesh/full.obj"}',
                //                                   planeEntity,
                //                                   "Model");
            }


        }
    }

    function loadOBJFile(url){
        //var fileName = "file:///D:/Dev/DLPSlicer/DLPslicer/resource/mesh/full.obj"
        var newObject = Qt.createQmlObject('import QtQuick 2.0; Model{inputSource:"'+url +'"}',
                                           planeEntity,
                                           "Model");
    }


    function axisAngle2Quaternion(angle, axis){
        var result = Qt.quaternion(0,0,0,0);
        result.x = axis.x * Math.sin(angle/2)
        result.y = axis.y * Math.sin(angle/2)
        result.z = axis.z * Math.sin(angle/2)
        result.scalar = Math.cos(angle/2)

        return result
    }

    function multiplyQuaternion(a,b)
    {
        var result = Qt.quaternion(0,0,0,0);
        result.x = a.x*b.scalar + a.scalar*b.x + a.y*b.z - a.z*b.y
        result.y = a.y*b.scalar + a.scalar*b.y + a.z*b.x - a.x*b.z
        result.z = a.z*b.scalar + a.scalar*b.z + a.x*b.y - a.y*b.x
        result.scalar = a.scalar*b.scalar - a.x*b.x - a.y*b.y - a.z*b.z

        return result
    }

    function openFile(){
        //fileDialog.open()
        console.log("button")
        var compo = Qt.createComponent("Model.qml");

        var loadmodel = compo.createObject(total, {});

    }

}
