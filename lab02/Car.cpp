#include "Car.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLUtils.hpp>
#include <iostream>

Car::Car( GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation ) {
    _position = glm::vec3( 40.0f, 0.5f, 15.0f );

    _shaderProgramHandle                            = shaderProgramHandle;
    _shaderProgramUniformLocations.mvpMtx           = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx        = normalMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor    = materialColorUniformLocation;

    _rotateAngle = _PI / 2.0f;

    _scaleBody = glm::vec3(1.0f,1.0f,2.0f);
    _colorBody = glm::vec3(1.0f,0.0f,0.0f);

    _scaleWheel = glm::vec3(0.3f,0.2f,0.3f);

    isMoving = false;
    isTurnLeft = false;
    isTurnRight = false;

    wheelRotation = 0.0f;

}
glm::vec3 Car::getPosition( ) { return _position; }
void Car::setPosition( glm::vec3 position ) { _position = position; }
glm::vec3 Car::getForwardDirection( ) { return _forwardDirection; }
void Car::setForwardDirection( ) { _forwardDirection = glm::vec3( cos( toRotate ), 0.0f, -sin( toRotate ) ); }
void Car::rotateSelf(GLfloat rotationFactor) {
    toRotate += rotationFactor;
}

void Car::notMoving() {
    isMoving = false;
}


void Car::moveForward( GLfloat movementFactor ) {
    _position += _forwardDirection * movementFactor;
    isMoving = true;
}

void Car::moveBackward( GLfloat movementFactor ) {
    _position -= _forwardDirection * movementFactor;
    isMoving = true;
}

bool Car::isFalling() const {
    return _isFalling;
}

void Car::draw(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) {
    modelMtx = glm::rotate(modelMtx, float(toRotate-M_PI/2), glm::vec3(0.0f, 1.0f, 0.0f));

    //modelMtx = glm::rotate( modelMtx, -_rotateAngle, CSCI441::Y_AXIS );
    //modelMtx = glm::rotate( modelMtx, _rotateAngle, CSCI441::Z_AXIS );

    _drawBody(modelMtx, viewMtx, projMtx);
    _drawWheels(modelMtx, viewMtx, projMtx);
}

void Car::_drawBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) {
    modelMtx = glm::scale( modelMtx, _scaleBody );

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, glm::value_ptr(_colorBody));

    CSCI441::drawSolidCube( 1.0f );

    glm::mat4 hoodMtx = glm::translate(modelMtx, glm::vec3(0.0f, -0.25f, -0.75f));
    hoodMtx = glm::scale(hoodMtx, glm::vec3(1.0f, 0.5f, 0.5f));
    _computeAndSendMatrixUniforms(hoodMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, glm::value_ptr(_colorBody));
    CSCI441::drawSolidCube( 1.0f );

    glm::mat4 windshieldMtx = glm::translate(modelMtx, glm::vec3(0.0f, 0.0f, -0.4f));
    windshieldMtx = glm::scale(windshieldMtx, glm::vec3(0.9f, 0.7f, 0.5f));
    windshieldMtx = glm::rotate(windshieldMtx, glm::radians(40.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    _computeAndSendMatrixUniforms(windshieldMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 1.0f)));
    CSCI441::drawSolidCube( 1.0f );
}

void Car::_drawWheel(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx, GLboolean isLeft) {
    modelMtx = glm::scale(modelMtx, _scaleWheel);
    modelMtx = glm::rotate(modelMtx, glm::radians(isLeft ? wheelRotation : -wheelRotation), glm::vec3(0.0f,1.0f,0.0f));
    fprintf(stdout, "Wheel rotation: %f\n", wheelRotation);

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
    CSCI441::drawSolidCylinder(1.0f,1.0f,1.0f,10,10);

    glm::mat4 wheelTopMtx = glm::translate(modelMtx, glm::vec3(0.0f,5* _scaleWheel.y, 0.0f));
    wheelTopMtx = glm::rotate(wheelTopMtx, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    _computeAndSendMatrixUniforms(wheelTopMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, glm::value_ptr(_colorBody));
    CSCI441::drawSolidDisk(0.0f,1.0f,10,10);

}

void Car::_drawWheels(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx)
{
    //Front Right
    glm::mat4 FR = glm::translate(modelMtx, glm::vec3(_scaleBody.x/2, -_scaleBody.y/2, -_scaleBody.z + 2*_scaleWheel.z));
    FR = glm::rotate(FR, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    if (isTurnRight || isTurnLeft)
    {
        FR = glm::rotate(FR, glm::radians(isTurnRight ? wheelRotationRate : -wheelRotationRate), glm::vec3(1.0f, 0.0f, 0.0f));
    }
    _drawWheel(FR, viewMtx, projMtx, false);
    //Front Left
    glm::mat4 FL = glm::translate(modelMtx, glm::vec3(-_scaleBody.x/2, -_scaleBody.y/2, -_scaleBody.z + 2*_scaleWheel.z));
    FL = glm::rotate(FL, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    if (isTurnRight || isTurnLeft)
    {
        FL = glm::rotate(FL, glm::radians(isTurnRight ? -wheelRotationRate : wheelRotationRate), glm::vec3(1.0f, 0.0f, 0.0f));
    }
    _drawWheel(FL, viewMtx, projMtx, true);
    //Back Right
    glm::mat4 BR = glm::translate(modelMtx, glm::vec3(_scaleBody.x/2, -_scaleBody.y/2, _scaleBody.z/2 - 2*_scaleWheel.z));
    BR = glm::rotate(BR, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    _drawWheel(BR, viewMtx, projMtx, false);
    //Back Left
    glm::mat4 BL = glm::translate(modelMtx, glm::vec3(-_scaleBody.x/2, -_scaleBody.y/2, _scaleBody.z/2 - 2*_scaleWheel.z));
    BL = glm::rotate(BL, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    _drawWheel(BL, viewMtx, projMtx, true);
}



void Car::setFalling(bool falling) {
    _isFalling = falling;
    if (falling) {
        // Initialize falling velocity
        _velocity = glm::vec3(0.0f);
    }
}
void Car::update() {
    if (_isFalling && false) {
      if(_deltaTime==0){
        _velocity = glm::vec3(0.0f);
        }
        // Apply gravity
        _deltaTime+=0.001f;
        const float GRAVITY = -9.81f; // Gravity acceleration (negative Y direction)
        _velocity.y += GRAVITY * _deltaTime; // Update velocity
        //printf("this is the velocity %f\n", _velocity.y);
        _position += _velocity * _deltaTime; // Update position
    }
    if(isMoving) {
        fprintf(stdout, "moving\n");
        wheelRotation += 1.0f;
    }

}



void Car::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex

    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.modelMtx, 1, GL_FALSE, glm::value_ptr(modelMtx) );

    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, glm::value_ptr(mvpMtx) );

    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, glm::value_ptr(normalMtx) );
}
