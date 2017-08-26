import QtQuick 2.0
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    property vector3d center : Qt.vector3d( 0.0, 20.0, 0.0 )
    Entity {
        id: sphere
        SphereMesh{
            id: sphere1
            radius : 10
        }
        Transform {
            id: sphere1Transform
            translation: center
            scale3D: Qt.vector3d(1,1,1)
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
            radius : 10.1
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
        PhongMaterial {
            id: torusXMaterial
            ambient: Qt.rgba(255/255,0/255,0/255,1.0)

        }

        components: [ torusXMesh, torusXMaterial, torusXTransform]
    }

    Entity {
        id: torusY
        TorusMesh{
            id: torusYMesh
            radius : 10.1
            minorRadius: 0.1
            rings: 100
            slices: 20

        }
        Transform {
            id: torusYTransform
            translation: center
            scale3D: Qt.vector3d(1,1,1)
            rotation:fromAxisAndAngle(Qt.vector3d(1,0, 0), 90)
        }
        PhongMaterial {
            id: torusYMaterial
            ambient: Qt.rgba(0/255,255/255,0/255,1.0)
        }

        components: [ torusYMesh, torusYMaterial, torusYTransform]
    }

    Entity {
        id: torusZ
        TorusMesh{
            id: torusZMesh
            radius : 10.1
            minorRadius: 0.1
            rings: 100
            slices: 20

        }
        Transform {
            id: torusZTransform
            translation: center
            scale3D: Qt.vector3d(1,1,1)
        }
        PhongMaterial {
            id: torusZMaterial
            ambient: Qt.rgba(0/255,0/255,255/255,1.0)
        }

        components: [ torusZMesh, torusZMaterial, torusZTransform]
    }
}
