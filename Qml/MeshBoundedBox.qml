import QtQuick 2.0
import QtQuick 2.0
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import Qt3D.Logic 2.0
import QtGraphicalEffects 1.0

Entity{
    id : boundedBox
    property vector3d center : Qt.vector3d( 0,0,0 )
    property vector3d inputScale : Qt.vector3d(0.6,0.6,0.6)
    property vector3d initScale : Qt.vector3d(0.3,0.3,0.3)

    property vector3d intersect : Qt.vector3d(0,0,0)
    property int moveAxis : 0 //0:none, 1:x, 2:y, 3:z
    property vector2d mouseOrigin : Qt.vector2d(0,0)
    property vector2d mouseCurrent : Qt.vector2d(0,0)
    property int moveDirection : 0

    property int pastDistance :0
    property int pastAxis :0


    Mesh{
        id: boundedBoxMesh
        source: "qrc:/Resource/mesh/boundedBox2.stl"
    }

    Transform {
        id:boundedBoxTransform
        translation:center
        scale3D: Qt.vector3d(1,1,1)
    }

    PhongMaterial{
        id : boundedBoxMaterial
        ambient: Qt.rgba(240/255, 249/255, 70/255, 1)
        diffuse: Qt.rgba(240/255, 249/255, 70/255, 1)
        specular:  Qt.rgba(240/255, 249/255, 70/255, 1)
        shininess: 0
    }

    components: [ boundedBoxMesh, boundedBoxTransform, boundedBoxMaterial]


    function hideBox(){
        enabled = false
    }

    function showBox(){
        enabled = true
    }

    function setSize(x,y,z){
        boundedBoxTransform.scale3D = Qt.vector3d(x/100,y/100,z/100)
    }

    function setPosition(center){
        boundedBoxTransform.translation = center
    }


}
