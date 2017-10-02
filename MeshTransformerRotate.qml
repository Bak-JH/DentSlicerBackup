import QtQuick 2.0
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    property vector3d center : Qt.vector3d( 450,0,450 )
    //property vector3d center : Qt.vector3d( 20 ,20,20 )
    property vector3d inputScale : Qt.vector3d(1,1,1)
    Transform {
        rotation : fromAxisAndAngle(Qt.vector3d(1,0, 1), 60)
    }

    Entity {
        id: sphere
        SphereMesh{
            id: sphere1
            radius : 14
        }
        Transform {
            id: sphere1Transform
            translation: center
            scale3D: inputScale

        }
        PhongAlphaMaterial{
            id: sphere1Material
            ambient: Qt.rgba(255/255,255/255,255/255,1.0)
            alpha: 0.01
        }

        components: [ sphere1, sphere1Material, sphere1Transform]
    }

    Entity {
        id: torusX
        TorusMesh{
            id: torusXMesh
            radius : 14.1
            minorRadius: 0.1
            rings: 100
            slices: 20

        }
        Transform {
            id: torusXTransform
            translation: center
            scale3D: Qt.vector3d(1,1,1)
            rotation: fromAxisAndAngle(Qt.vector3d(0,1, 0), 90)
        }
        PhongAlphaMaterial {
            id: torusXMaterial
            ambient: Qt.rgba(0/255,0/255,0/255,0.5)

        }
        ObjectPicker{
            id : objectPickerX
             hoverEnabled: true
             onContainsMouseChanged: {
             }

             onEntered: {
                torusXMaterial.ambient = Qt.rgba(237/255,217/255,41/255,1.0)
             }

             onExited: {
                torusXMaterial.ambient = Qt.rgba(0/255,0/255,0/255,0.35)
             }
         }
        components: [ torusXMesh, torusXMaterial, torusXTransform,objectPickerX]
    }

    Entity {
        id: torusY
        TorusMesh{
            id: torusYMesh
            radius : 14.1
            minorRadius: 0.3
            rings: 100
            slices: 20

        }
        Transform {
            id: torusYTransform
            translation: center
            scale3D: inputScale
            rotation:fromAxisAndAngle(Qt.vector3d(1,0, 0), 90)
        }
        PhongAlphaMaterial {
            id: torusYMaterial
            ambient: Qt.rgba(0/255,0/255,0/255,0.5)
        }

        ObjectPicker{
            id : objectPickerY
             hoverEnabled: true

             onEntered: {
                 torusYMaterial.ambient = Qt.rgba(237/255,217/255,41/255,0.35)
             }

             onExited: {
                 torusYMaterial.ambient = Qt.rgba(0/255,0/255,0/255,0.35)
             }
         }

        components: [ torusYMesh, torusYMaterial, torusYTransform, objectPickerY]
    }

    Entity {
        id: torusZ
        TorusMesh{
            id: torusZMesh
            radius : 14.1
            minorRadius: 0.3
            rings: 100
            slices: 20

        }
        Transform {
            id: torusZTransform
            translation: center
            scale3D: inputScale
        }
        PhongAlphaMaterial {
            id: torusZMaterial
            ambient: Qt.rgba(0/255,0/255,0/255,0.5)
        }

        ObjectPicker{
            id : objectPickerZ
            hoverEnabled: true

            onEntered: {
                torusZMaterial.ambient = Qt.rgba(237/255,217/255,41/255,1.0)
            }

             onExited: {       
                torusZMaterial.ambient = Qt.rgba(0/255,0/255,0/255,0.35)
             }
         }
        components: [ torusZMesh, torusZMaterial, torusZTransform,objectPickerZ]
    }

    function perfectPosition(){
        var temp, cameraP, cameraV ;
        cameraP = sceneRoot.cm.camera.position;
        cameraV = sceneRoot.cm.camera.viewCenter;

        temp = cameraP.minus(cameraV).normalized()
        center = cameraP.minus(temp.times(400))
        //sceneRoot.total.mtr.center = camera.position.minus(temp.times(400))
    }
}
