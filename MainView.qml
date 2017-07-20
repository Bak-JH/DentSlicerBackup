
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import QtQuick 2.7 as QQ2


import Qt3D.Logic 2.0

Entity {
    id: sceneRoot
    property vector3d xup : Qt.vector3d( 1.0, 0.0, 0.0 )
    property vector3d xdown : Qt.vector3d( -1.0, 0.0, 0.0 )

    property vector3d yup : Qt.vector3d( 0.0, 1.0, 0.0 )
    property vector3d ydown : Qt.vector3d( 0.0, -1.0, 0.0 )
    property vector3d zup : Qt.vector3d( 0.0, 0.0, 1.0 )
    property vector3d zdown : Qt.vector3d( 0.0, 0.0, -1.0 )

    CoordinateMesh{}
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

            if(d>0)
                cm.zoomUp()
            else if(d<0)
                cm.zoomDown()
        }
    }


    /*
    GoochMaterial {
        id: planeMaterial
        warmColor: Qt.rgba(40/255,40/255,40/255,1.0)
        coolColor: Qt.rgba(240/255,240/255,240/255,1.0)
    }*/
    DiffuseMapMaterial{
        id : planeMaterial
        diffuse: "qrc:/grid2.png"
        specular: Qt.rgba( 0.9, 0.9, 0.9, 1.0 )
        shininess: 2.0
    }

    DiffuseMapMaterial{
        id : planeDownMaterial
        diffuse: "qrc:/grid.png"
        specular: Qt.rgba( 0.9, 0.9, 0.9, 1.0 )
        shininess: 2.0
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

        Entity {
            id: meshEntity
            Mesh{
                id: testMesh
                source: "file:///D:/Dev/Trash/3Dtest/lowerjaw.obj"
                //source: "file:///D:/Dev/Trash/3Dtest/full.stl"
            }
            Transform {
                id: meshTransform
                property quaternion objectRotation : fromAxisAndAngle(Qt.vector3d(0,0, 1), 60)

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
            Entity {
                PlaneMesh{
                    id: planeDownMesh
                    height : 100
                    width : 100

                }

                Transform{
                    id: planeDownTransform
                    translation: Qt.vector3d(0,0,0)
                    rotation: fromAxisAndAngle(Qt.vector3d(1,0, 0), 180)
                }

                components: [ planeDownMesh, planeDownMaterial, planeDownTransform ]
            }
            Entity {
                id: cylinderEntity1
                CylinderMesh{
                    id: cylinderMesh1
                    radius: 0.1
                    length: 100
                }

                Transform{
                    id: cylinderTransform1
                    translation: Qt.vector3d(-50,cylinderMesh1.length/2,50)
                    rotation: fromAxisAndAngle(Qt.vector3d(1,0, 0), 0)
                }

                components: [ cylinderMesh1, planeMaterial, cylinderTransform1 ]
            }

            Entity {
                id: cylinderEntity2
                CylinderMesh{
                    id: cylinderMesh2
                    radius: 0.1
                    length: 100

                }

                Transform{
                    id: cylinderTransform2
                    translation: Qt.vector3d(-50,cylinderMesh2.length/2,-50)
                    rotation: fromAxisAndAngle(Qt.vector3d(1,0, 0), 0)
                }

                components: [ cylinderMesh2, planeMaterial, cylinderTransform2 ]
            }

            Entity {
                id: cylinderEntity3
                CylinderMesh{
                    id: cylinderMesh3
                    radius: 0.1
                    length: 100

                }

                Transform{
                    id: cylinderTransform3
                    translation: Qt.vector3d(50,cylinderMesh3.length/2,50)
                    rotation: fromAxisAndAngle(Qt.vector3d(1,0, 0), 0)
                }

                components: [ cylinderMesh3, planeMaterial, cylinderTransform3 ]
            }

            Entity {
                id: cylinderEntity4
                CylinderMesh{
                    id: cylinderMesh4
                    radius: 0.1
                    length: 100

                }

                Transform{
                    id: cylinderTransform4
                    translation: Qt.vector3d(50,cylinderMesh2.length/2,-50)
                    rotation: fromAxisAndAngle(Qt.vector3d(1,0, 0), 0)
                }

                components: [ cylinderMesh4, planeMaterial, cylinderTransform4 ]
            }


            Entity {
                id: backPlane
                PlaneMesh{
                    id: backPlaneMesh
                    height: 100
                    width: 100

                }

                Transform{
                    id: backPlaneTransform
                    translation: Qt.vector3d(-50,backPlaneMesh.height/2,0)
                    rotation: fromAxisAndAngle(Qt.vector3d(0,0, 1), -90)
                }

                GoochMaterial {
                    id: backPlaneMaterial
                    warmColor: Qt.rgba(234/255,234/255,234/255,1.0)
                    coolColor: Qt.rgba(234/255,234/255,234/255,1.0)
                }

                components: [ backPlaneMesh, backPlaneMaterial, backPlaneTransform ]
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
                    rotation: fromAxisAndAngle(Qt.vector3d(1,0, 0), 0)
                }

                components: [ planeTitleMesh, planeMaterial, planeTitleTransform ]
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
                    var target = axisAngle2Quaternion(rotationSpeed * rotateXAxis.value * dt,qq.rotatedVector(planeTransform.rotation,yup))
                    var target2 = axisAngle2Quaternion(rotationSpeed * rotateYAxis.value * dt,ydown)

                    //console.log("rtv " + qq.rotatedVector(planeTransform.rotation,ydown))
                    target = qq.multiplyQuaternion(target,target2)

                    planeTransform.rotation = qq.multiplyQuaternion(target,planeTransform.rotation)
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
}
