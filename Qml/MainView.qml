
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import QtQuick 2.7
//import GLQML 1.0


import Qt3D.Logic 2.0

Entity {
    property vector3d xup : Qt.vector3d( 1.0, 0.0, 0.0 )
    property vector3d xdown : Qt.vector3d( -1.0, 0.0, 0.0 )

    property vector3d yup : Qt.vector3d( 0.0, 1.0, 0.0 )
    property vector3d ydown : Qt.vector3d( 0.0, -1.0, 0.0 )
    property vector3d zup : Qt.vector3d( 0.0, 0.0, 1.0 )
    property vector3d zdown : Qt.vector3d( 0.0, 0.0, -1.0 )

    property alias total: total
    property alias cm: cm
    property alias systemTransform: systemTransform
    property alias keyboardHandler: keyboardHandler

    //CoordinateMesh{} // 기준좌표 체크

    CameraManager{id : cm}


    Entity{
        id : total
        objectName:"total"
        Transform{
            id: systemTransform
            objectName : "systemTransform"
            scale3D: Qt.vector3d(0.004,0.004,0.004)
            rotationX : -70
            rotationY : 0
            rotationZ : -40
        }

        components: [systemTransform]

        Plate{
            id: planeEntity
        }

        Model{
            id: meshEntity4
            //inputSource:"file:///D:/Dev/DLPSlicer/DLPslicer/resource/mesh/lowerjaw.obj"
        }

        MeshBoundedBox{
            objectName: "boundedBox"
            enabled: false
        }

        Entity{
            objectName: "rotateSphereEntity"
            MeshTransformerRotate{
                objectName: "rotateSphere"
            }
        }
        Entity{
            objectName: "moveArrowEntity"
            MeshTransformerMove{
                objectName: "moveArrow"
            }
        }

        CoordinateMesh{}
    }


    KeyboardDevice{
        id : keyboardDevice
    }

    signal copy();
    signal paste();
    signal unDo();
    signal reDo();
    signal groupSelectionActivate(bool b);

    KeyboardHandler{
        focus : true
        id : keyboardHandler
        sourceDevice: keyboardDevice

        onPressed: {
            console.log(event.key);
            if (event.key === Qt.Key_Delete) {
                deletePopUp.targetID = qm.getselectedModelID()
                if (deletePopUp.targetID != -1){
                    deletePopUp.visible = true
                    mttab.hideTab();
                }
            } else if (event.key === Qt.Key_Escape) {
                uppertab.all_off()
            }else if (event.matches(StandardKey.Undo)){
                // do undo
                console.log("undo called");
                unDo();
            } else if (event.matches(StandardKey.Redo)){
                // do redo
                console.log("redo called");
                reDo();
            } else if (event.matches(StandardKey.Open)){
                openFile();
            } else if (event.matches(StandardKey.Copy)){
                copy();
            } else if (event.matches(StandardKey.Paste)){
                paste();
            } else if (event.key === Qt.Key_Shift){
                console.log("shift pressed");
                groupSelectionActivate(true);
            }
        }
        onReleased:{
            if (event.key === Qt.Key_Shift){
                console.log("shift released");
                groupSelectionActivate(false);
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

    function openFile(){
        var compo = Qt.createComponent("Model.qml");

        var loadmodel = compo.createObject(total, {});
    }

    function initCamera(){
        cm.initCamera()
    }

}
