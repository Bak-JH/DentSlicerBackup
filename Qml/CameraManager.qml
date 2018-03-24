import QtQuick 2.0

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import QtQuick 2.7 as QQ2


Entity {
    property real xLookAtOffset: 0
    property real yLookAtOffset: 0
    property real zLookAtOffset: 0
    property real xCameraOffset: 0
    property real yCameraOffset: 0
    property real zCameraOffset: 0

    property vector3d defaultUp: Qt.vector3d(0, 1, 0)
    property vector3d defaultDown: Qt.vector3d(0,-1, 0)
    property vector3d defaultCameraPosition: Qt.vector3d(0,0,100)
    //property vector3d orthoCameraPosition: Qt.vector3d(5000,0,5000)
    //property vector3d defaultCameraPosition2: Qt.vector3d(580,0,580)

    property vector3d inputViewCenter : Qt.vector3d( 0, 0, 0)
    property alias camera: camera


    Camera {
        id: camera

        projectionType: CameraLens.OrthographicProjection
        fieldOfView: 100
        nearPlane : 10
        farPlane : 1000

        position: defaultCameraPosition
        upVector: defaultUp

        viewCenter: inputViewCenter

        property vector3d temp : Qt.vector3d( 0.0, 0.0, 0.0 )

        onPositionChanged: {
            cameraLightTransform.translation = camera.position.times(0.2)
            cameraLight.localDirection = camera.viewCenter.minus(camera.position).times(100)
        }
    }

    Entity {
        components: [
            SpotLight {
                id: cameraLight
                localDirection: Qt.vector3d(0.0, 0.0, -5.0)
                color: "white"
                intensity: 0.6
            },
            Transform {
                id: cameraLightTransform
                translation: Qt.vector3d(0.0, 0.0, 5.0)

            }
        ]
    }

    Entity {
        components: [
            DirectionalLight {
                //worldDirection: Qt.vector3d(0.3, -3.0, 0.0).normalized();
                worldDirection : Qt.vector3d(0, 0, -1).normalized()

                //color: "#fbf9ce"
                color: "white"
                intensity: 0.3
            }
        ]
    }

    CameraController{
        camera:camera
    }

    components: [
        RenderSettings {
            id : rd
            pickingSettings.pickMethod: PickingSettings.TrianglePicking
            pickingSettings.pickResultMode: PickingSettings.AllPicks

            activeFrameGraph: RenderSurfaceSelector {
                Viewport {
                    id: mainViewport
                    normalizedRect: Qt.rect(0, 0, 1, 1)

                    ClearBuffers {
                        buffers: ClearBuffers.ColorDepthBuffer
                        clearColor: Qt.rgba(0.6, 0.6, 0.6, 1.0)
                    }

                    Viewport {
                        id: mainCam
                        normalizedRect: Qt.rect(0, 0, 1, 1)
                        CameraSelector {
                            camera:camera
                            ClearBuffers {
                               buffers: ClearBuffers.ColorDepthBuffer
                               clearColor: "#EAEAEA"
                            }
                            SortPolicy {
                                sortTypes: [
                                    SortPolicy.BackToFront
                                ]
                            }
                        }
                    }
                }
            }
        },

        InputSettings {}
    ]


    QQ2.Vector3dAnimation{
        id : zoomAnimation
        target: camera
        property : "position"

        from : camera.position
        //to: camera.position.minus(camera.temp)
        duration: 100
        easing.type: "Linear"

    }

    function zoomGap(){
        var Camera_position, average, temp, cameraRadius

        Camera_position = camera.position;
        average = camera.viewCenter
        temp = Camera_position.minus(average.times(10))
        cameraRadius = Math.sqrt(Math.pow(temp.x,2) + Math.pow(temp.y,2) + Math.pow(temp.z,2))

        camera.temp = temp.times(0.25)

        return cameraRadius
    }

    function zoomUp(){
        var cameraRadius = zoomGap()

        if(cameraRadius > 10){
            /*
            //camera.position = camera.position.minus(temp.times(0.25))

            zoomAnimation.to = camera.position.minus(camera.temp)
            zoomAnimation.start()
            */
            var zoomFactor = 0.1
            var zoom = 1.0 - zoomFactor;
            var viewVector = camera.position.minus(camera.viewCenter)
            viewVector = viewVector.times(zoom);
            camera.position = camera.viewCenter.plus(viewVector);
            //sceneRoot.total.mtr.perfectPosition();

        }

    }

    function zoomDown(){
        var cameraRadius = zoomGap()

        if(cameraRadius < 1600){
            /*
            //camera.position = camera.position.minus(temp.times(0.25))
            zoomAnimation.to = camera.position.plus(camera.temp)
            zoomAnimation.start()
            */

            var zoomFactor = 0.1
            var zoom = 1.0 + zoomFactor;
            var viewVector = camera.position.minus(camera.viewCenter)
            viewVector = viewVector.times(zoom);
            camera.position = camera.viewCenter.plus(viewVector);

            //sceneRoot.total.mtr.perfectPosition();
        }

    }

    MouseDevice{
        id : mouseDevice
    }

    MouseHandler{
        id : mouseHandler
        sourceDevice: mouseDevice
    }

    KeyboardDevice{
        id : keyboardDevice
    }
    KeyboardHandler{
        focus : true
        id : keyboardHandler
        sourceDevice: keyboardDevice

        property vector3d temp;

        onPressed: {
            if (event.key === Qt.Key_A) {
                camera.position = camera.position.plus(Qt.vector3d(0,0,-10))
            }
            if (event.key === Qt.Key_S) {
                camera.position = camera.position.plus(Qt.vector3d(0,-10,0))
            }
            if (event.key === Qt.Key_D) {
                camera.position = camera.position.plus(Qt.vector3d(0,0,10))
            }
            if (event.key === Qt.Key_W) {
                camera.position = camera.position.plus(Qt.vector3d(0,10,0))
            }

            if (event.key === Qt.Key_V) {

            }
        }
    }

}
