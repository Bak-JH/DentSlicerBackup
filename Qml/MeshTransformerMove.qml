import QtQuick 2.0
import QtQuick 2.0
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import Qt3D.Logic 2.0
import QtGraphicalEffects 1.0

Entity{
    id : moveArrow
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
    components: [
        Transform {
            id:moveArrowTransform
            translation:center
            scale3D: Qt.vector3d(1,1,1)
        }
    ]
    Mesh{
        id: moveArrowMesh
        source: "qrc:/Resource/mesh/arrow.stl"
    }
    /*Entity {
            id: spheremove
            objectName: "moveSphere"
            SphereMesh{
                id: spheremesh
                radius : 5
            }
            Transform {
                id: sphere3Transform
                translation: Qt.vector3d( 0,0,0 )
            }
            PhongAlphaMaterial{
                id: sphere3Material
                ambient: Qt.rgba(255/255,255/255,255/255,0.5)
            }

            components: [ spheremesh, sphere3Material, sphere3Transform]
        }*/
    Entity {
        id: moveArrowX
        Transform{
            id: movearrowXTransform
            translation: Qt.vector3d(0,0,0)
            scale3D: initScale
            rotation: fromAxisAndAngle(Qt.vector3d(0,1, 0), 90)
        }
        PhongAlphaMaterial{
            id : moveArrowXMaterial
            ambient: Qt.rgba(50/255, 50/255, 50/255, 0.5)
            shininess: 0
        }
        ObjectPicker{
            id : moveArrowobjectPickerX
            dragEnabled: true
            hoverEnabled: true
            onEntered: {
                if (moveAxis == 0){
                    moveArrowXMaterial.ambient = Qt.rgba(150/255,150/255,0/255,0.5) //rotation end
                }
            }
            onExited: {
                if (moveAxis == 0){
                    moveArrowXMaterial.ambient = Qt.rgba(50/255,50/255,50/255,0.5) //rotation end
                }
            }
            onPressed: { //move start
                if (moveAxis  == 0){
                    intersect = Qt.vector3d(pick.localIntersection.x,pick.localIntersection.y,pick.localIntersection.z)

                    if (pick.localIntersection.z > 0){
                        moveDirection = 1;
                    }else{
                        moveDirection = -1;
                    }

                    mouseOrigin = Qt.vector2d(pick.position.x , pick.position.y)
                    moveAxis = 1 //X
                    moveArrowXMaterial.ambient = Qt.rgba(200/255,200/255,0/255,1)
                    pastDistance=0;
                }
            }
            onReleased: {
                moveAxis = 0;
                moveArrowXMaterial.ambient = Qt.rgba(50/255,50/255,50/255,0.5) //rotation end
            }
        }
        components: [ moveArrowMesh, movearrowXTransform, moveArrowXMaterial,moveArrowobjectPickerX ]
    }
    Entity {
        id: moveArrowY
        Transform{
            id: movearrowYTransform
            translation: Qt.vector3d(0,0,0)
            scale3D: initScale
            rotation: fromAxisAndAngle(Qt.vector3d(1,0,0), 90)
        }
        PhongAlphaMaterial{
            id : moveArrowYMaterial
            ambient: Qt.rgba(50/255, 50/255, 50/255, 0.5)
            shininess: 0
        }
        ObjectPicker{
            id : moveArrowobjectPickerY
            dragEnabled: true
            hoverEnabled: true
            onEntered: {
                if (moveAxis == 0){
                    moveArrowYMaterial.ambient = Qt.rgba(150/255,150/255,0/255,0.5) //rotation end
                }
            }
            onExited: {
                if (moveAxis == 0){
                    moveArrowYMaterial.ambient = Qt.rgba(50/255,50/255,50/255,0.5) //rotation end
                }
            }
            onPressed: { //move start
                if (moveAxis  == 0){
                    intersect = Qt.vector3d(pick.localIntersection.x,pick.localIntersection.y,pick.localIntersection.z)

                    if (pick.localIntersection.z > 0){
                        moveDirection = -1;
                    }else{
                        moveDirection = 1;
                    }
                    mouseOrigin = Qt.vector2d(pick.position.x , pick.position.y)
                    moveAxis = 2 //Y
                    moveArrowYMaterial.ambient = Qt.rgba(200/255,200/255,0/255,1)
                    pastDistance=0;
                }
            }
            onReleased: {
                moveAxis = 0;
                moveArrowYMaterial.ambient = Qt.rgba(50/255,50/255,50/255,0.5) //rotation end
            }
        }
        components: [ moveArrowMesh, movearrowYTransform, moveArrowYMaterial,moveArrowobjectPickerY]
    }
    MouseDevice{
        id: movemousedevice
        sensitivity: 0.01
    }
    MouseHandler{
        id : moveMouseHandler
        sourceDevice: movemousedevice
        onClicked: {
            if (mouse.buttons == MouseEvent.RightButton){
                moveAxis = 0;
            }
        }
        onReleased: {
            if (mouse.buttons == MouseEvent.LeftButton){
                moveAxis = 0;
                moveArrowXMaterial.ambient = Qt.rgba(50/255,50/255,50/255,0.5)
                moveArrowYMaterial.ambient = Qt.rgba(50/255,50/255,50/255,0.5)
                moveArrowZMaterial.ambient = Qt.rgba(50/255,50/255,50/255,0.5)
            }
        }
        onPositionChanged: {
            mouseCurrent = Qt.vector2d(mouse.x,mouse.y)
        }
    }
    function world2Screen(target){
        var tmp = Qt.vector3d(0,0,0)
        tmp = target
        target = tmp.times(systemTransform.scale3D)
        var point = target
        var matrix = Qt.matrix4x4(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
        matrix = sceneRoot.cm.camera.projectionMatrix.times(sceneRoot.cm.camera.viewMatrix);
        point = matrix.times(point)
        point.x = (point.x+1) * scene3d.width/2;
        point.y = (-1 * point.y+1) * scene3d.height/2;
        return Qt.vector2d(point.x,point.y)
    }
    signal moveSignal(int Axis, int distance)
    signal moveDone(int Axis)
    FrameAction{
        id : moveFrameAction
        onTriggered: {
            if (parent.parent.enabled.toString()=="true"){
                var syszoom = Qt.vector3d(0,0,0)
                syszoom = sceneRoot.systemTransform.scale3D
                if (moveAxis == 0 && pastAxis != 0){
                    moveDone(pastAxis);
                    pastAxis = 0;
                }
                pastAxis = moveAxis
                if (moveAxis != 0){
                    console.log(intersect)
                    console.log(moveDirection)
                    var Origin = world2Screen(center)
                    var mouseOrigin_Origin = mouseOrigin.minus(Origin).length() //a
                    var mouseCurrent_Origin = mouseCurrent.minus(Origin).length() //b
                    var mouseCurrent_mouseOrigin = mouseOrigin.minus(mouseCurrent).length() //c
                    var distance = ((Math.pow(mouseCurrent_Origin,2)-Math.pow(mouseCurrent_mouseOrigin,2)-Math.pow(mouseOrigin_Origin,2))/(2*mouseOrigin_Origin))
                    distance*=( 0.0006 / syszoom.x)
                    switch(moveAxis){
                    case 1: // xAxis
                        moveSignal(1,moveDirection*(distance - pastDistance))
                        break;
                    case 2: // yAxis
                        moveSignal(2,moveDirection*(distance - pastDistance))
                        break;
                    }
                    pastDistance = distance
                    //sphere3Transform.translation = Qt.vector3d(distance,0,0)
                    //console.log(distance)
                }
                moveArrowTransform.scale3D = Qt.vector3d(0.01/syszoom.x,0.01/syszoom.y,0.01/syszoom.z)
                moveArrowTransform.translation = center.minus(cm.camera.viewVector)
            }else{
                moveAxis=0;
            }
        }
    }
}
