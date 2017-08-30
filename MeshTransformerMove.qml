import QtQuick 2.0
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    property vector3d center : Qt.vector3d( 0.0, 20.0, 0.0 )
    Entity {
        id: arrow
        Mesh{
            id: arrowMesh
            source:"file:///D:/Dev/DLPSlicer/DLPslicer/resource/mesh/arrow.obj"
        }
        Transform {
            id: arrowTransform
            translation: center
            scale3D: Qt.vector3d(1,1,1)
        }
        PhongAlphaMaterial{
            id: arrowMaterial
            ambient: Qt.rgba(255/255,255/255,0/255,1.0)
            alpha: 0.8
        }

        components: [ arrowMesh, arrowMaterial, arrowTransform]
    }

    Entity {
        id: arrowY
        Mesh{
            id: arrowYMesh
            source:"file:///D:/Dev/DLPSlicer/DLPslicer/resource/mesh/arrow.obj"
        }
        Transform {
            id: arrowYTransform
            translation: center
            scale3D: Qt.vector3d(1,1,1)
            rotation: fromAxisAndAngle(Qt.vector3d(0,0, 1), 90)
        }
        PhongAlphaMaterial{
            id: arrowYMaterial
            ambient: Qt.rgba(0/255,255/255,0/255,1.0)
            alpha: 0.8
        }
        /*
        ObjectPicker{
            id : arrowYpicker
            hoverEnabled: true
            onEntered: {
                arrowYMaterial.ambient = Qt.rgba(255/255,0/255,0/255,1.0)
                console.log("in")
            }

            onExited: {
                arrowYMaterial.ambient = Qt.rgba(0/255,255/255,0/255,1.0)
                console.log("out")
            }
            onContainsMouseChanged: {
                console.log("cfcfccfcfcf");
            }
        }
        */
        components: [ arrowYMesh, arrowYMaterial, arrowYTransform]
    }

}
