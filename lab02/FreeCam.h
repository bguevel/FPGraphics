#ifndef LAB02_FREE_CAM_H
#define LAB02_FREE_CAM_H

#include <CSCI441/Camera.hpp>
#include <CSCI441/PerspectiveCamera.hpp>

class FreeCam final : public CSCI441::PerspectiveCamera {
public:
    void recomputeOrientation() final;

    void moveForward(GLfloat movementFactor) final;
    void moveBackward(GLfloat movementFactor) final;

private:
    void _updateFreeCameraPosition();
};

#endif // LAB02_FREE_CAM_H