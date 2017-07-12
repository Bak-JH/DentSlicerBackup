
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import QtQuick 2.7 as QQ2


import Qt3D.Logic 2.0

Entity {
    id: sceneRoot

    CameraManager{
        id : cm



    }

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

            if(d>0){
                cm.zoomUp()

            }
            else if(d<0){
                cm.zoomDown()
            }

        }
    }



    GoochMaterial {
        id: planeMaterial
        warmColor: Qt.rgba(40/255,40/255,40/255,1.0)
        coolColor: Qt.rgba(240/255,240/255,240/255,1.0)
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
    CoordinateMesh{}
    Entity{
        id : total

        Transform{
            id : totalTransform

            rotation: fromAxisAndAngle(Qt.vector3d(0,0, 1), 60)
        }

        Entity {
            id: meshEntity
            Mesh{
                id: testMesh
                source: "file:///D:/Dev/Trash/3Dtest/lowerjaw.obj"
                //source: "file:///D:/Dev/Trash/3Dtest/full.stl"
            }
            Transform {
                id: meshTransform
                property vector3d objectRotation : fromAxisAndAngle(Qt.vector3d(0,0, 1), 60)

                scale3D: Qt.vector3d(0.1,0.1,0.1)

                rotation: objectRotation
                translation: Qt.vector3d(0,0,0)
            }
            /*
            ObjectPicker{
                onClicked: {
                    console.log("click")
                }
            }*/

            components: [ testMesh, meshMaterial, meshTransform,picker ]

        }
        /*
        MeshTest{

        }*/


        Entity {
            id: planeEntity
            PlaneMesh{
                id: planeMesh
                height : 100
                width : 100

            }

            Transform{
                id: planeTransform
                translation: Qt.vector3d(0,0,0)
                rotation: fromAxisAndAngle(Qt.vector3d(1,0, 0), 90)
            }

            components: [ planeMesh, planeMaterial, planeTransform ]
        }

        Entity {
            id: planeTitleEntity
            PlaneMesh{
                id: planeTitleMesh
                height : 60
                width : 10

            }

            Transform{
                id: planeTitleTransform
                translation: Qt.vector3d(50,0,0)
                rotation: fromAxisAndAngle(Qt.vector3d(1,0, 0), 90)
            }

            components: [ planeTitleMesh, planeMaterial, planeTitleTransform ]
        }
        /*
        function changeRotation(){
            var temp = meshTransform.rotation
            //var temp2 = Qt.quaternion(0.996,0,0,0.087)
            var temp2 = Qt.quaternion(0.996,0,0.087,0)

            console.log(temp + "   " + temp2)
            var target = multiplyQuaternion(temp,temp2)

        }

        function multiplyQuaternion(a,b)
        {
            var result = Qt.quaternion(0,0,0,0);
            result.x = a.x*b.scalar + a.scalar*b.x + a.y*b.z - a.z*b.y
            result.y = a.y*b.scalar + a.scalar*b.y + a.z*b.x - a.x*b.z
            result.z = a.z*b.scalar + a.scalar*b.z + a.x*b.y - a.y*b.x
            result.scalar = a.scalar*b.scalar - a.x*b.x - a.y*b.y - a.z*b.z

            meshTransform.rotation = result
            return result

        }
        */
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
            property real rotationSpeed : 10
            onTriggered: {
                if (rotateAction.active) {
                    var target = axisAngle2Quaternion(rotationSpeed * rotateXAxis.value * dt,Qt.vector3d( 0.0, 1.0, 0.0 ))
                    //var target2 = axisAngle2Quaternion(rotationSpeed * rotateYAxis.value * dt,Qt.vector3d( 0.0, 0.0, 1.0 ))
                    planeTransform.rotation = multiplyQuaternion(planeTransform.rotation,target)
                    meshTransform.rotation = multiplyQuaternion(meshTransform.rotation,planeTransform.rotation)
                    //meshTransform.rotation = multiplyQuaternion(meshTransform.rotation,target2)


                    //meshTransform.rotation = target2;
                    /*
                    root.camera.panAboutViewCenter(rotationSpeed * rotateXAxis.value * dt,Qt.vector3d( 0.0, 0.0, 1.0 ));
                    root.camera.tiltAboutViewCenter(rotationSpeed * rotateYAxis.value * dt,Qt.vector3d( -1.0, 0.0, 0.0 ));
                    // lock the camera roll angle
                    root.camera.setUpVector(_originalUpVector);
                    */
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
            /*
            if (event.key === Qt.Key_W) {
                meshTransform.translation = meshTransform.translation.plus(Qt.vector3d(0.00,0.05,0.00));
            }
            if (event.key === Qt.Key_S) {
                meshTransform.translation = meshTransform.translation.minus(Qt.vector3d(0.00,0.05,0.00));

            }
            if (event.key === Qt.Key_A) {
                meshTransform.translation = meshTransform.translation.plus(Qt.vector3d(0.05,0.00,0.00));
            }
            if (event.key === Qt.Key_D) {
                meshTransform.translation = meshTransform.translation.minus(Qt.vector3d(0.05,0.00,0.00));

            }*/


            if (event.key === Qt.Key_J) {
                //totalTransform.rotation = Qt.quaternion(10,1,0,0)
                console.log(meshTransform.rotation)
            }
            if (event.key === Qt.Key_K) {
                //totalTransform.rotation = Qt.quaternion(20,1,0,0)
                total.changeRotation()
            }
            if (event.key === Qt.Key_I) {
                //totalTransform.rotation = Qt.quaternion(30,1,0,0)
                meshTransform.rotation = Qt.quaternion(0.5,0,0,0.5)
            }
            if (event.key === Qt.Key_L) {
                   //totalTransform.rotation = Qt.quaternion(30,1,0,0)
                meshTransform.rotation = Qt.quaternion(0.8,0,0,0.5)
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

        meshTransform.rotation = result
        return result

    }
}
