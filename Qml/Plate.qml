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


    Entity{
        Mesh{
            id: platePlaneMesh
            source: "qrc:/Resource/mesh/plate_plane.stl"
        }

        Transform{
            id: platePlaneTransform
            translation: Qt.vector3d(0,0,0)
            //rotation: fromAxisAndAngle(Qt.vector3d(1,0, 0), 90)
        }
        components: [ platePlaneMesh, platePlaneTransform, platePlaneMaterial ]
    }

    Entity{
        Mesh{
            id: plateMesh
            source: "qrc:/Resource/mesh/plate.stl"
        }

        Transform{
            id: plateTransform
            translation: Qt.vector3d(0,0,0)
            //rotation: fromAxisAndAngle(Qt.vector3d(1,0, 0), 90)
        }
        components: [ plateMesh, plateTransform, plateMaterial ]
    }

}
