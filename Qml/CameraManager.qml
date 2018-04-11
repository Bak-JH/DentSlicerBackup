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
            pickingSettings.pickResultMode: PickingSettings.NearestPick

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

    function initCamera(){
        sceneRoot.cm.camera.translateWorld(sceneRoot.cm.camera.viewCenter.times(-1))
        sceneRoot.cm.camera.translateWorld(Qt.vector3d(-0.015,-0.16,0))
    }
}
