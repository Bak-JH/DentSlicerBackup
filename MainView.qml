
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import QtQuick 2.7


import Qt3D.Logic 2.0

Entity {

    property vector3d xup : Qt.vector3d( 1.0, 0.0, 0.0 )
    property vector3d xdown : Qt.vector3d( -1.0, 0.0, 0.0 )

    property vector3d yup : Qt.vector3d( 0.0, 1.0, 0.0 )
    property vector3d ydown : Qt.vector3d( 0.0, -1.0, 0.0 )
    property vector3d zup : Qt.vector3d( 0.0, 0.0, 1.0 )
    property vector3d zdown : Qt.vector3d( 0.0, 0.0, -1.0 )

    CoordinateMesh{} // 기준좌표 체크

    CameraManager{id : cm}

    MouseDevice{
        id : mouseDevice
    }

    MouseHandler{
        id : mouseHandler
        sourceDevice: mouseDevice

        onPressed: {
            //console.log("cliccckckc")
        }

        onWheel: {
            var Camera_position, average, temp, Camera_radius
            var d = wheel.angleDelta.y ;

            if(d>0)
                cm.zoomUp()
            else if(d<0)
                cm.zoomDown()
        }
    }

    DiffuseMapMaterial{
        id : planeMaterial
        /*
        ambient: Qt.rgba( 204/255, 204/255, 204/255, 1.0 )
        diffuse: "black"
        specular: "black"
        shininess: 0.0
        */

        diffuse: "qrc:/grid.png"
        //specular: Qt.rgba( 1, 1, 1, 1.0 )
        ambient: Qt.rgba(255/255, 255/255, 255/255, 1.0 )
        shininess: 0

    }


    NormalDiffuseMapAlphaMaterial{
        id : planeDownMaterial
        diffuse: "qrc:/grid2.png"
        normal: "qrc:/grid2.png"
        ambient: Qt.rgba(0/255, 0/255, 0/255, 0.0)
        //specular: Qt.rgba( 0, 0, 0, 1.0 )
        shininess: 0
    }

    PhongMaterial {
        id: meshMaterial
        property int counter : 0
        ambient: Qt.rgba(81/255,200/255,242/255,1.0)

    }



    ObjectPicker {
        id: picker
        hoverEnabled: true
        //onClicked: console.log("Contains mouse: " + containsMouse)
        /*
        onContainsMouseChanged: console.log("Contains mouse: " + containsMouse)
        */

        onClicked: {
            if(meshMaterial.counter%2 == 0)
                meshMaterial.ambient = Qt.rgba(200/255,200/255,242/255,1.0)
            else
                meshMaterial.ambient = Qt.rgba(81/255,200/255,242/255,1.0)
            meshMaterial.counter++;
        }

        onContainsMouseChanged: {
            meshMaterial.ambient = Qt.rgba(200/255,40/255,242/255,1.0)
        }
        onExited: {
            meshMaterial.ambient = Qt.rgba(200/255,200/255,242/255,1.0)
        }
    }

    Entity{
        id : total

        Transform{
            id : totalTransform
            //rotation: fromAxisAndAngle(Qt.vector3d(0,0, 1), 60)
        }
        /*
        Model{
            id: meshEntity
            inputSource:"file:///D:/Dev/Trash/3Dtest/lowerjaw.obj"
        }

        Model{
            id: meshEntity2
            inputSource:"file:///D:/Dev/DLPSlicer/DLPslicer/resource/mesh/hix.obj"
        }
        Model{
            id: meshEntity3
            inputSource:"file:///D:/Dev/DLPSlicer/DLPslicer/resource/mesh/hix.obj"
        }*/

        Plane{id: planeEntity
            Model{
                id: meshEntity4
                inputSource:"file:///D:/Dev/DLPSlicer/DLPslicer/resource/mesh/lowerjaw.obj"
            }
        }
    }

    MouseDevice {
        id: mouse
        sensitivity: 0.1
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
            property real rotationSpeed : 4

            onTriggered: {
                if (rotateAction.active) {

                    var target = axisAngle2Quaternion(rotationSpeed * rotateXAxis.value * dt,qq.rotatedVector(planeEntity.planeTransform.rotation,yup))
                    var target2 = axisAngle2Quaternion(rotationSpeed * rotateYAxis.value * dt,ydown)

                    //console.log("rtv " + qq.rotatedVector(planeTransform.rotation,ydown))
                    target = qq.multiplyQuaternion(target,target2)

                    planeEntity.planeTransform.rotation = qq.multiplyQuaternion(target,planeEntity.planeTransform.rotation)
                    //planeTransform.rotation = qq.multiplyQuaternion(planeTransform.rotation,target)

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
                console.log(meshTransform.rotation)
            }
            if (event.key === Qt.Key_K) {
                total.changeRotation()
            }
            if (event.key === Qt.Key_I) {
                meshTransform.rotation = Qt.quaternion(0.5,0,0,0.5)
            }
            if (event.key === Qt.Key_L) {
                meshTransform.rotation = Qt.quaternion(0.8,0,0,0.5)
            }
            if (event.key === Qt.Key_A) {
                meshEntity4.testMesh.setSource("file:///D:/Dev/DLPSlicer/DLPslicer/resource/mesh/hix.obj");
                //openFile();
            }
            if (event.key === Qt.Key_S) {
                meshEntity4.testMesh.setSource("file:///D:/Dev/DLPSlicer/DLPslicer/resource/mesh/lowerjaw.obj");
            }
            if (event.key === Qt.Key_D) {
                meshEntity4.testMesh.setSource("file:///D:/Dev/DLPSlicer/DLPslicer/resource/mesh/upperjaw.obj");
            }
        }
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
