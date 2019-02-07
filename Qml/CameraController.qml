/*
  CameraController.qml

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Daniel Vrátil <daniel.vratil@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


import Qt3D.Core 2.0
import Qt3D.Input 2.0
import Qt3D.Logic 2.0
import Qt3D.Render 2.0

Entity {
    id: root

    property Camera camera

    property vector3d _originalUpVector

    onCameraChanged: {
        if (root.camera) {
            // we don't want to create a binding, but a copy of the initial value
            _originalUpVector = root.camera.upVector;
        }
    }

    MouseDevice {
        id: mouse
        sensitivity: 0.005
    }

    /***************************************** Mouse Right Rotate *****************************************/
    components: [
        LogicalDevice {
              id: logicalDevice;

              actions: [
                  Action {
                      id: rotateAction
                      inputs: [
                          ActionInput {
                              sourceDevice: mouse
                              buttons: [ MouseEvent.RightButton ]
                          }
                      ]
                  }
              ]


              axes: [
                  Axis {
                      id: rotateXAxis
                      inputs: [
                          AnalogAxisInput {
                              sourceDevice: mouse
                              axis: MouseDevice.X
                          }
                      ]
                  },
                  Axis {
                      id: rotateYAxis
                      inputs: [
                          AnalogAxisInput {
                              sourceDevice: mouse
                              axis: MouseDevice.Y
                          }

                      ]
                  }
              ]
          },

        FrameAction {
            property real rotationSpeed : 2000
            onTriggered: {
                if (rotateAction.active) {// mouse right rotate
                    if (qm.freecutActive)
                        return ;
                    console.log("rotation:",sceneRoot.systemTransform.rotation, rotateXAxis.value, rotateYAxis.value);
                    sceneRoot.systemTransform.rotationZ += rotationSpeed * rotateXAxis.value * dt;
                    //var target = axisAngle2Quaternion(rotationSpeed * rotateXAxis.value * dt,qq.rotatedVector(systemTransform.rotation,zdown))
                    //cm.camera.rotateAboutViewCenter(qq.multiplyQuaternion(target,systemTransform.rotation));
                    sceneRoot.systemTransform.rotationX += rotationSpeed * (-1) * rotateYAxis.value * dt;

                    mttab.updatePosition()
                }
            }

        }
    ]

    /***************************************** Mouse Wheel Zoom *****************************************/
    // => the function moved to main.qml
    /*
    MouseHandler{
        id : mouseHandler
        sourceDevice: mouse

        onWheel: {
            //console.log(wheel.x, wheel.y);
            //var Camera_position, average, temp, Camera_radius
            var d = wheel.angleDelta.y ;
            var scaleTmp = sceneRoot.systemTransform.scale3D;

            if(d>0){// mouse wheel zoom
                sceneRoot.systemTransform.scale3D = scaleTmp.times(1.08);
                mttab.updatePosition();
            }
            else if(d<0){
                sceneRoot.systemTransform.scale3D = scaleTmp.times(0.92);
                mttab.updatePosition();
            }
        }

        onPressed: {
            if(mouse.button ===1){ //left click
                qm.setModelClickFalse();
            }

        }

        onReleased: {
            if(mouse.button ===1){ //left click
                qm.backgroundClickCheck();
            }
        }
    }
    */

    /***************************************** Mouse Wheel Move *****************************************/
    // => main.qml MouseArea

}
