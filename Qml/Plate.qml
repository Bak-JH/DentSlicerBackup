import QtQuick 2.0
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    PhongAlphaMaterial{
        id : plateMaterial
        ambient: Qt.rgba(100/255, 100/255, 100/255, 1 )
        diffuse: Qt.rgba(100/255, 100/255, 100/255, 1 )
        specular: Qt.rgba(100/255, 100/255, 100/255, 1 )
        shininess: 0
        alpha: 1
    }

    PhongAlphaMaterial{
        id : platePlaneMaterial
        ambient: Qt.rgba(255/255, 255/255, 255/255, 1 )
        diffuse: Qt.rgba(255/255, 255/255, 255/255, 1 )
        specular: Qt.rgba(255/255, 255/255, 255/255, 1 )
        alpha: 1
    }

    PhongAlphaMaterial{
        id : plateLogoMaterial
        ambient: Qt.rgba(255/255, 0/255, 0/255, 1 )
        diffuse: Qt.rgba(255/255, 0/255, 0/255, 1 )
        specular: Qt.rgba(255/255, 0/255, 0/255, 1 )
        alpha: 1
    }


    Entity{
        id:gridPlateEntity
        PlaneMesh {
            id: gridPlateMesh
            width: qm.getBedXSize();
            height: qm.getBedYSize();
            meshResolution: Qt.size(2,2);
        }

        Transform{
            id: gridPlateTransform
            translation: Qt.vector3d(0,0,-0.02)
            rotationX : 90
            //scale3D: Qt.vector3d(30/100,6/80,0)
        }

        components:[gridPlateTransform, gridPlateMesh, platePlaneMaterial]

    }

    Entity{
        id: plateLogoFrontPlaneEntity
        Mesh{
            id: plateLogoFrontPlaneMesh
            source: "qrc:/Resource/mesh/plate_plane.stl"
        }

        Transform{
            id: plateLogoFrontPlaneTransform
            translation: Qt.vector3d(0,-qm.getBedYSize()/2-3,0)
            scale3D: Qt.vector3d(30/100,6/80,0)

        }
        components: [ plateLogoFrontPlaneMesh, plateLogoFrontPlaneTransform, platePlaneMaterial ]
    }

    Entity{
        id: plateLogoBackPlaneEntity
        Mesh{
            id: plateLogoBackPlaneMesh
            source: "qrc:/Resource/mesh/plate_plane.stl"
        }

        Transform{
            id: plateLogoBackPlaneTransform
            translation: Qt.vector3d(0,-qm.getBedYSize()/2-3,0)
            scale3D: Qt.vector3d(30/100,6/80,0)
            rotationX : -180
        }
        components: [ plateLogoBackPlaneMesh, plateLogoBackPlaneTransform, platePlaneMaterial ]
    }

    Entity{
        id : plateLogoEntity
        Mesh{
            id: plateLogoMesh
            source: "qrc:/Resource/mesh/plate_logo.stl"
        }

        Transform{
            id: plateLogoTransform
            translation: Qt.vector3d(0,-qm.getBedYSize()/2-3,0)
        }
        components: [ plateLogoMesh, plateLogoTransform, plateMaterial ]
    }
}
