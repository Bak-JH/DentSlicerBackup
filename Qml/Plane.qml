import QtQuick 2.0
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import QtQml.Models 2.1

Entity {
    property alias planeTransform: planeTransform

    PlaneMesh{
        id: planeMesh
        height : 100
        width : 80

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
            width : 80

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
            length: 150
        }

        Transform{
            id: cylinderTransform1
            translation: Qt.vector3d(-40,cylinderMesh1.length/2,50)
            rotation: fromAxisAndAngle(Qt.vector3d(1,0, 0), 0)
        }

        components: [ cylinderMesh1, planeMaterial, cylinderTransform1 ]
    }

    Entity {
        id: cylinderEntity2
        CylinderMesh{
            id: cylinderMesh2
            radius: 0.1
            length: 150

        }

        Transform{
            id: cylinderTransform2
            translation: Qt.vector3d(-40,cylinderMesh2.length/2,-50)
            rotation: fromAxisAndAngle(Qt.vector3d(1,0, 0), 0)
        }

        components: [ cylinderMesh2, planeMaterial, cylinderTransform2 ]
    }

    Entity {
        id: cylinderEntity3
        CylinderMesh{
            id: cylinderMesh3
            radius: 0.1
            length: 150

        }

        Transform{
            id: cylinderTransform3
            translation: Qt.vector3d(40,cylinderMesh3.length/2,50)
            rotation: fromAxisAndAngle(Qt.vector3d(1,0, 0), 0)
        }

        components: [ cylinderMesh3, planeMaterial, cylinderTransform3 ]
    }

    Entity {
        id: cylinderEntity4
        CylinderMesh{
            id: cylinderMesh4
            radius: 0.1
            length: 150

        }

        Transform{
            id: cylinderTransform4
            translation: Qt.vector3d(40,cylinderMesh2.length/2,-50)
            rotation: fromAxisAndAngle(Qt.vector3d(1,0, 0), 0)
        }

        components: [ cylinderMesh4, planeMaterial, cylinderTransform4 ]
    }


    /*Entity {
        id: backPlane
        PlaneMesh{
            id: backPlaneMesh
            height: 150
            width: 80

        }

        Transform{
            id: backPlaneTransform
            //translation: Qt.vector3d(-50,75,0)
            rotation: fromAxisAndAngle(Qt.vector3d(1,0, 0), 90)
            translation: Qt.vector3d(0,75,-50)
        }

        GoochMaterial {
            id: backPlaneMaterial
            warmColor: Qt.rgba(244/255,244/255,244/255,1.0)
            coolColor: Qt.rgba(244/255,244/255,244/255,1.0)
        }

        components: [ backPlaneMesh, backPlaneMaterial, backPlaneTransform ]
    }*/

    Entity {
        id: planeTitleEntity
        PlaneMesh{
            id: planeTitleMesh
            height : 10
            width : 60

        }

        Transform{
            id: planeTitleTransform
            translation: Qt.vector3d(0,0,50)
            rotation: fromAxisAndAngle(Qt.vector3d(1,0, 0), 0)
        }

        components: [ planeTitleMesh, planeMaterial, planeTitleTransform ]
    }

    //MeshTransformerRotate{}
    //MeshTransformerMove{}



}
