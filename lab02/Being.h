#ifndef LAB05_BEING_H
#define LAB05_BEING_H

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class Being
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
    Being( GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation );

    /// \desc draws the model plane for a given MVP matrix
    /// \param modelMtx existing model matrix to apply to plane
    /// \param viewMtx camera view matrix to apply to plane
    /// \param projMtx camera projection matrix to apply to plane
    /// \note internally uses the provided shader program and sets the necessary uniforms
    /// for the MVP and Normal Matrices as well as the material diffuse color
    void drawPerson( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx );

    /// \desc simulates the plane flying forward by rotating the propeller clockwise

    void rotateSelf( GLfloat rotationFactor );
    void moveNose( );

    glm::vec3 getPosition( );
    void setPosition( glm::vec3 position );
    glm::vec3 getForwardDirection( );
    void setForwardDirection( );
    void moveForward( GLfloat movementFactor );
    void moveBackward( GLfloat movementFactor );
    float toRotate = 0;

  private:
    glm::vec3 _forwardDirection;
    glm::vec3 _position;
    int moveNoseBy = 0;
    glm::vec3 facing;
    /// \desc current angle of rotation for the propeller
    GLfloat _propAngle;
    /// \desc one rotation step
    GLfloat _propAngleRotationSpeed;

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
        GLint materialColor;

        GLint modelMtx;
    } _shaderProgramUniformLocations;

    /// \desc angle to rotate our plane at
    GLfloat _rotatePlaneAngle;

    /// \desc color the plane's body
    glm::vec3 _colorBody;
    /// \desc amount to scale the plane's body by
    glm::vec3 _scaleBody;

    /// \desc color the plane's wing
    glm::vec3 _colorWing;
    /// \desc amount to scale the plane's wing by
    glm::vec3 _scaleWing;
    /// \desc amount to rotate the plane's wing by
    GLfloat _rotateWingAngle;

    /// \desc color the plane's nose
    glm::vec3 _colorNose;
    /// \desc amount to rotate the plane's nose by
    GLfloat _rotateNoseAngle;

    /// \desc color the plane's propeller
    glm::vec3 _colorProp;
    /// \desc amount to scale the plane's propeller by
    glm::vec3 _scaleProp;
    /// \desc amount to translate the plane's propeller by
    glm::vec3 _transProp;

    /// \desc color the plane's tail
    glm::vec3 _colorTail;

    const GLfloat _PI        = glm::pi<float>( );
    const GLfloat _2PI       = glm::two_pi<float>( );
    const GLfloat _PI_OVER_2 = glm::half_pi<float>( );

    void _drawLegs( bool isLeftWing, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;
    void _drawHead( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;
    /// \desc draws just the plane's body
    /// \param modelMtx existing model matrix to apply to plane
    /// \param viewMtx camera view matrix to apply to plane
    /// \param projMtx camera projection matrix to apply to plane
    void _drawBody( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;

    /// \desc draws a single wing
    /// \param isLeftWing true if left wing, false if right wing (controls wing rotation)
    /// \param modelMtx existing model matrix to apply to plane
    /// \param viewMtx camera view matrix to apply to plane
    /// \param projMtx camera projection matrix to apply to plane

    void _drawArms( bool isLeftWing, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;

    /// \desc draws the nose of the plane
    /// \param modelMtx existing model matrix to apply to plane
    /// \param viewMtx camera view matrix to apply to plane
    /// \param projMtx camera projection matrix to apply to plane

    void _drawNose( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;

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
};

#endif // LAB05_BEING_H
