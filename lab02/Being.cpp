#include "Being.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLUtils.hpp>

Being::Being( GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation ) {
    _position = glm::vec3( 40.0f, 0.5f, 15.0f );

    _shaderProgramHandle                            = shaderProgramHandle;
    _shaderProgramUniformLocations.mvpMtx           = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx        = normalMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor    = materialColorUniformLocation;

    _rotatePlaneAngle = _PI / 2.0f;

    _colorBody = glm::vec3( 0.0f, 0.0f, 1.0f );
    _scaleBody = glm::vec3( 2.0f, 0.5f, 1.0f );

    _colorWing = glm::vec3( 1.0f, 0.0f, 0.0f );
    _scaleWing = glm::vec3( 1.5f, 0.5f, 1.0f );
    _rotateWingAngle = _PI / 2.0f;

    _colorNose = glm::vec3( 0.0f, 1.0f, 0.0f );
    _rotateNoseAngle = _PI / 2.0f;

    _colorProp = glm::vec3( 1.0f, 1.0f, 1.0f );
    _scaleProp = glm::vec3( 1.1f, 1.0f, 0.025f );
    _transProp = glm::vec3( 0.1f, 0.0f, 0.0f );

    _colorTail = glm::vec3( 1.0f, 1.0f, 0.0f );
}
glm::vec3 Being::getPosition( ) { return _position; }
void Being::setPosition( glm::vec3 position ) { _position = position; }
glm::vec3 Being::getForwardDirection( ) { return _forwardDirection; }
void Being::setForwardDirection( ) { _forwardDirection = glm::vec3( cos( toRotate ), 0.0f, -sin( toRotate ) ); }
void Being::rotateSelf(GLfloat rotationFactor) {
    toRotate += rotationFactor;
}
void Being::moveForward( GLfloat movementFactor ) { _position += _forwardDirection * movementFactor; }

void Being::moveBackward( GLfloat movementFactor ) { _position -= _forwardDirection * movementFactor; }
void Being::moveNose() {
    moveNoseBy = (moveNoseBy+1)%6;
}

bool Being::isFalling() const {
    return _isFalling;
}

void Being::setFalling(bool falling) {
    _isFalling = falling;
    if (falling) {
        // Initialize falling velocity
        _velocity = glm::vec3(0.0f);
    }
}
void Being::update() {
    if (_isFalling) {
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
}

void Being::drawPerson( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) {

    modelMtx = glm::rotate(modelMtx, float(toRotate-M_PI/2), glm::vec3(0.0f, 1.0f, 0.0f));

    modelMtx = glm::rotate( modelMtx, -_rotatePlaneAngle, CSCI441::Y_AXIS );
    modelMtx = glm::rotate( modelMtx, _rotatePlaneAngle, CSCI441::Z_AXIS );

    _drawBody(modelMtx, viewMtx, projMtx);        // the body of our plane
    _drawArms(true, modelMtx, viewMtx, projMtx);  // the left wing
    _drawArms(false, modelMtx, viewMtx, projMtx); // the right wing
    _drawHead(modelMtx, viewMtx, projMtx);
    _drawLegs(true, modelMtx, viewMtx, projMtx);
    _drawLegs(false, modelMtx, viewMtx, projMtx);
    _drawNose(modelMtx, viewMtx, projMtx);        // the nose


}

void Being::_drawBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::scale( modelMtx, _scaleBody );

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, glm::value_ptr(_colorBody));

    CSCI441::drawSolidCube( 0.3f );
}

void Being::_drawArms(bool isLeftWing, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::translate(modelMtx, glm::vec3(0 , 0, (isLeftWing ? -0.2f : 0.2f) ));
    modelMtx = glm::scale( modelMtx, _scaleBody );
    modelMtx = glm::rotate( modelMtx, (isLeftWing ? -1.f : 1.f) * _rotateWingAngle, CSCI441::X_AXIS );

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, glm::value_ptr(_colorWing));
    CSCI441::drawSolidCube( 0.2f );

}

void Being::_drawNose(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::translate(modelMtx, glm::vec3(0.5,0+float(moveNoseBy/10.0f),0));

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, glm::value_ptr(_colorNose));

    CSCI441::drawSolidCone( 0.025f, 0.3f, 16, 16 );
}
void Being::_drawHead(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::translate(modelMtx, glm::vec3(0.5,0,0));

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, glm::value_ptr(_colorNose));

   CSCI441::drawSolidSphere( 0.15f, 16, 16 );
}
void Being::_drawLegs(bool isLeftWing, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::translate(modelMtx, glm::vec3(-0.4 , 0, (isLeftWing ? -0.11f : 0.11f) ));
    modelMtx = glm::scale( modelMtx, _scaleBody );
    modelMtx = glm::rotate( modelMtx, (isLeftWing ? -1.f : 1.f) * _rotateWingAngle, CSCI441::X_AXIS );

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, glm::value_ptr(_colorWing));
    CSCI441::drawSolidCube( 0.15f );

}

void Being::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex

    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.modelMtx, 1, GL_FALSE, glm::value_ptr(modelMtx) );

    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, glm::value_ptr(mvpMtx) );

    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, glm::value_ptr(normalMtx) );
}
