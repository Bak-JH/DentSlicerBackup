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
            scale3D: Qt.vector3d(0.2,0.2,0.2)
        }
        PhongAlphaMaterial{
            id: arrowXMaterial
            ambient: Qt.rgba(255/255,255/255,0/255,1.0)
            alpha: 0.8
        }

        ObjectPicker{
            id : arrowXpicker
            hoverEnabled: true
            onEntered: {
                arrowXMaterial.ambient = Qt.rgba(80/255,80/255,80/255,1.0)
                console.log("inX")
            }

            onExited: {
                arrowXMaterial.ambient = Qt.rgba(255/255,255/255,0/255,1.0)
                console.log("outX")
            }
            onClicked: {
                console.log("clickX")
            }
            onReleased: {
                console.log("releaseX")
            }
        }

        components: [ arrowMesh, arrowXMaterial, arrowTransform, arrowXpicker]
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
            scale3D: Qt.vector3d(0.2,0.2,0.2)
            rotation: fromAxisAndAngle(Qt.vector3d(0,0, 1), 90)
        }
        PhongAlphaMaterial{
            id: arrowYMaterial
            ambient: Qt.rgba(0/255,255/255,0/255,1.0)
            alpha: 0.8
        }

        ObjectPicker{
            id : arrowYpicker
            hoverEnabled: true
            onEntered: {
                arrowYMaterial.ambient = Qt.rgba(80/255,80/255,80/255,1.0)
                console.log("inY")
            }

            onExited: {
                arrowYMaterial.ambient = Qt.rgba(0/255,255/255,0/255,1.0)
                console.log("outY")
            }
            onClicked: {
                console.log("clickY")
            }
            onReleased: {
                console.log("releasey")
            }

        }

        components: [ arrowYMesh, arrowYMaterial, arrowYTransform, arrowYpicker]
    }

}
