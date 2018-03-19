import QtQuick 2.0
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    Mesh{
        id: plateMesh
        source: "qrc:/Resource/mesh/grid.obj"

    }

    Transform{
        id: plateTransform
        translation: Qt.vector3d(0,0,0)
        //rotation: fromAxisAndAngle(Qt.vector3d(1,0, 0), 90)
    }

    PhongMaterial{
        id : plateMaterial
        ambient: Qt.rgba(255/255, 255/255, 255/255, 0 )
        diffuse: Qt.rgba(255/255, 255/255, 255/255, 0 )
        specular: Qt.rgba(255/255, 255/255, 255/255, 0 )
        shininess: 0
    }

    components: [ plateMesh, plateTransform, plateMaterial ]
}
