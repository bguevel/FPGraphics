#ifndef FP_CAR_H
#define FP_CAR_H

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "CSCI441/materials.hpp"

class Car
{
  public:
    bool _isFalling = false;
    glm::vec3 _velocity;
    bool isFalling() const;
    void setFalling(bool falling);
    void update();
    float _deltaTime = 0;
    /// \desc creates a simple plane that gives the appearance of flight
    /// \param shaderProgramHandle shader program handle that the plane should be drawn using
    /// \param mvpMtxUniformLocation uniform location for the full precomputed MVP matrix
    /// \param normalMtxUniformLocation uniform location for the precomputed Normal matrix
    /// \param materialColorUniformLocation uniform location for the material diffuse color
    Car( GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation,  GLint materialDiffuseUniformLocation, GLint materialSpecularUniformLocation, GLint materialShineUniformLocation, GLint emitterUniformLocation, GLboolean isAI);

    /// \desc draws the model plane for a given MVP matrix
    /// \param modelMtx existing model matrix to apply to plane
    /// \param viewMtx camera view matrix to apply to plane
    /// \param projMtx camera projection matrix to apply to plane
    /// \note internally uses the provided shader program and sets the necessary uniforms
    /// for the MVP and Normal Matrices as well as the material diffuse color
    void draw( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx );
    void _drawBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx);
    void _drawWheel(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx, GLboolean isLeft);
    void _drawWheels(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx);
    void _drawHeadlights(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx);

    /// \desc simulates the plane flying forward by rotating the propeller clockwise

    void rotateSelf( GLfloat rotationFactor );

    glm::vec3 getPosition( );
    void setPosition( glm::vec3 position );
    glm::vec3 getForwardDirection( );
    void setForwardDirection( );
    void setForwardDirection(const glm::vec3& direction);
    void moveForward( GLfloat movementFactor );
    void moveBackward( GLfloat movementFactor );
    void notMoving();
    float toRotate = 0;

    GLboolean isTurnLeft;
    GLboolean isTurnRight;
    GLboolean isMoving;


  private:
    glm::vec3 _forwardDirection;
    glm::vec3 _position;
    glm::vec3 facing;

    glm::vec3 _scaleBody;
    glm::vec3 _colorBody;

    glm::vec3 _scaleWheel;



    GLfloat wheelTurn;
    GLfloat wheelRotation;

    GLfloat bounce = 0.0f;
    GLboolean isUp = true;
    GLfloat bounceRate = 0.01f;
    GLfloat bounceLimit = 0.2f;

    GLfloat wheelRotationRate = 30.0f;

    /// \desc handle of the shader program to use when drawing the plane
    GLuint _shaderProgramHandle;

    /// \desc stores the uniform locations needed for the plan information
    struct ShaderProgramUniformLocations
    {
        /// \desc location of the precomputed ModelViewProjection matrix
        GLint mvpMtx;
        /// \desc location of the precomputed Normal matrix
        GLint normalMtx;
        /// \desc location of the material diffuse color
        GLint materialDiffuse;
        GLint materialSpecular;
        GLint materialShine;
        GLint isEmitter;

        GLint modelMtx;
    } _shaderProgramUniformLocations;

    /// \desc angle to rotate our plane at
    GLfloat _rotateAngle;

    GLboolean isAI;

    const GLfloat _PI        = glm::pi<float>( );
    const GLfloat _2PI       = glm::two_pi<float>( );
    const GLfloat _PI_OVER_2 = glm::half_pi<float>( );


    /// \desc draws the two propeller pieces
    /// \param modelMtx existing model matrix to apply to plane
    /// \param viewMtx camera view matrix to apply to plane
    /// \param projMtx camera projection matrix to apply to plane

    /// \desc precomputes the matrix uniforms CPU-side and then sends them
    /// to the GPU to be used in the shader for each vertex.  It is more efficient
    /// to calculate these once and then use the resultant product in the shader.
    /// \param modelMtx model transformation matrix
    /// \param viewMtx camera view matrix
    /// \param projMtx camera projection matrix
    void _computeAndSendMatrixUniforms( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;
    void _sendMaterial(const CSCI441::Materials::Material&) const;
};

#endif // LAB05_BEING_H
