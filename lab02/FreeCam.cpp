#include "FreeCam.h"
#include<cmath>

void FreeCam::recomputeOrientation() {
    // TODO #2
    mCameraDirection = glm::vec3 { sin(mCameraTheta)*sin(mCameraPhi), -1*cos(mCameraPhi), -1*cos(mCameraTheta)*sin(mCameraPhi)};
    _updateFreeCameraPosition();
}

void FreeCam::moveForward(GLfloat movementFactor) {
    mCameraRadius += movementFactor;
    recomputeOrientation();
}

void FreeCam::moveBackward(GLfloat movementFactor) {
    mCameraRadius -= movementFactor;
    recomputeOrientation();
}

void FreeCam::_updateFreeCameraPosition() {
    // TODO #3
    mCameraPosition = (mCameraDirection*mCameraRadius)+mCameraLookAtPoint;
    computeViewMatrix();
}
