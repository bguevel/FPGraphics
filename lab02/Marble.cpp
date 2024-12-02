#include "Marble.h"

#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/OpenGLUtils.hpp>
#include <CSCI441/objects.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdlib>

Marble::Marble()
    : _rotation(0.0f), _location(glm::vec3(0,0,0)), _direction(glm::vec3(1,0,0)),
      RADIUS(0.5f),
      SPEED(0.01f),
      COLOR(glm::vec3(_genRandColor(), _genRandColor(), _genRandColor())) {

}


Marble::Marble(glm::vec3 location, glm::vec3 direction, GLfloat radius, GLuint shaderProgramHandle,
               GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation,
               GLint materialColorUniformLocation, GLint viewMtxUniformLocation)
    : _rotation(0.0f), _location(location), _direction(direction),
      RADIUS(radius), SPEED(0.03f),
      COLOR(glm::vec3(_genRandColor(), _genRandColor(), _genRandColor())),
      _shaderProgramHandle(shaderProgramHandle) {
    _direction = glm::normalize(_direction);
    _shaderProgramUniformLocations.mvpMtx = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx = normalMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor = materialColorUniformLocation;
    _shaderProgramUniformLocations.viewMtx = viewMtxUniformLocation; // Store the viewMtx uniform location
}


glm::vec3 Marble::getLocation() const { return _location; }
void Marble::setLocationX(GLfloat x) { _location.x = x; }
void Marble::setLocationZ(GLfloat z) { _location.z = z; }
glm::vec3 Marble::getDirection() const { return _direction; }
void Marble::setDirection(glm::vec3 newDirection) { _direction = glm::normalize(newDirection); }

void Marble::draw(glm::mat4 modelMtx, glm::mat4 projMtx, glm::mat4 viewMtx) {
    modelMtx = glm::translate(modelMtx, _location);
    modelMtx = glm::translate(modelMtx, glm::vec3(0, RADIUS, 0));
    glm::vec3 rotationAxis = glm::cross(_direction, CSCI441::Y_AXIS);
    modelMtx = glm::rotate(modelMtx, _rotation, rotationAxis);

    // Compute and send mvpMatrix
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    glProgramUniformMatrix4fv(_shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, glm::value_ptr(mvpMtx));

    // Compute and send normal matrix
    glm::mat3 normalMtx = glm::mat3(glm::transpose(glm::inverse(modelMtx)));
    glProgramUniformMatrix3fv(_shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, glm::value_ptr(normalMtx));

    // **Compute and send the model-view matrix**
    glm::mat4 modelViewMtx = viewMtx * modelMtx;
    glProgramUniformMatrix4fv(_shaderProgramHandle, _shaderProgramUniformLocations.viewMtx, 1, GL_FALSE, glm::value_ptr(modelViewMtx));

    // Set the material color
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, glm::value_ptr(COLOR));

    // Draw the sphere
    CSCI441::drawSolidSphere(RADIUS, 16, 16);
}

bool Marble::isFalling() const {
    return _isFalling;
}

void Marble::setFalling(bool falling) {
    _isFalling = falling;
    if (falling) {
        // Initialize falling velocity (if needed)
        _velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    }
}
void Marble::update() {
    if (_isFalling) {
        // Apply gravity
        _deltaTime+=0.01f;
        const float GRAVITY = -9.81f; // Gravity acceleration (negative Y direction)
        _velocity.y += GRAVITY * _deltaTime; // Update velocity
        _location += _velocity * _deltaTime; // Update position
    } else {
        moveForward(); // Continue moving normally
    }
}
void Marble::moveForward() {
  if(!_isFalling){
      _location += _direction * SPEED;
      _rotation -= SPEED;
      if( _rotation < 0.0f ) {
          _rotation += 6.28f;
      }
  }
}

void Marble::moveBackward() {
    _location -= _direction * SPEED;
    _rotation += SPEED;
    if( _rotation > 6.28f ) {
        _rotation -= 6.28f;
    }
}

GLfloat Marble::_genRandColor() {
    return (GLfloat)(rand() * 100 % 50) / 100.0f + 0.5f;
}