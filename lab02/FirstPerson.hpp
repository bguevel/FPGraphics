#include <CSCI441/PerspectiveCamera.hpp>
#include <glm/glm.hpp>
//#include <glm/gtx/string_cast.hpp>
#include <iostream>

#ifndef _FIRST_PERSON_H_
#define _FIRST_PERSON_H_

class FirstPerson final : public CSCI441::PerspectiveCamera {
public:
  FirstPerson( GLfloat aspectRatio = 1.0f, GLfloat fovy = 45.0f, GLfloat nearClipPlane = 0.001f, GLfloat farClipPlane = 1000.0f );

  void updatePosition( glm::vec3 position, glm::vec3 forwardDirection );

  void setCameraPosition( glm::vec3 position );
  void setCameraLookAtPoint( glm::vec3 lookAtPoint );

  void moveForward( GLfloat movementFactor );
  void moveBackward( GLfloat movementFactor );

  void recomputeOrientation( );
    void setCameraDirection(glm::vec3 direction );

};

inline FirstPerson::FirstPerson(GLfloat aspectRatio, GLfloat fovy, GLfloat nearClipPlane, GLfloat farClipPlane)
    : CSCI441::PerspectiveCamera(aspectRatio, fovy, nearClipPlane, farClipPlane) {}

inline void FirstPerson::updatePosition(glm::vec3 position, glm::vec3 forwardDirection) {
    setCameraPosition(position);
    setCameraLookAtPoint(position + forwardDirection);
}
inline void FirstPerson::setCameraDirection( glm::vec3 direction ) { mCameraDirection = direction; }

inline void FirstPerson::setCameraPosition(glm::vec3 position) {
    mCameraPosition = position;
}

inline void FirstPerson::setCameraLookAtPoint(glm::vec3 lookAtPoint) {
    mCameraLookAtPoint = lookAtPoint;
}

inline void FirstPerson::moveForward(GLfloat movementFactor) {
    mCameraPosition += mCameraDirection * movementFactor;
    mCameraLookAtPoint += mCameraDirection * movementFactor;
}

inline void FirstPerson::moveBackward(GLfloat movementFactor) {
    mCameraPosition -= mCameraDirection * movementFactor;
    mCameraLookAtPoint -= mCameraDirection * movementFactor;
}

inline void FirstPerson::recomputeOrientation( )
{
  // Calculate the forward direction vector using spherical coordinates
  mCameraDirection.x = cos( mCameraTheta ) * cos( mCameraPhi );
  mCameraDirection.y = sin( mCameraPhi );
  mCameraDirection.z = sin( mCameraTheta ) * cos( mCameraPhi );
  mCameraDirection   = glm::normalize( mCameraDirection );

  mCameraLookAtPoint = mCameraPosition + mCameraDirection;
  mCameraUpVector    = glm::vec3( 0.0f, 1.0f, 0.0f );

  computeViewMatrix( );
}

#endif // _FIRST_PERSON_H_
