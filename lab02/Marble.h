#ifndef LAB11_MARBLE_H
#define LAB11_MARBLE_H

#include <glm/glm.hpp>

#include <glad/gl.h>

#include <CSCI441/ShaderProgram.hpp>

/// \class Marble
/// \desc Stores the information for a colored sphere that has a position, direction, size, and orientation
class Marble {
public:
    bool _isFalling = false;
    glm::vec3 _velocity;
    bool isFalling() const;
    void setFalling(bool falling);
    void update();
    float _deltaTime=0;
    /// \desc creates a Marble of unit size at the origin moving along X with a random color
    Marble();
    /// \desc creates a Marble for the given specifications with random color
    /// \param location location the Marble begins at
    /// \param direction direction the Marble is initially moving
    /// \param radius radius of the marble
    Marble(glm::vec3 location, glm::vec3 direction, GLfloat radius, GLuint shaderProgramHandle,
               GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation,
               GLint materialColorUniformLocation, GLint viewMtxUniformLocation);

    /// \desc radius of the sphere
    const GLfloat RADIUS;
    /// \desc speed the sphere is moving
    const GLfloat SPEED;
    /// \desc color the sphere is tinted (RGB)
    const glm::vec3 COLOR;

    /// \desc get the location of the sphere center
    /// \returns center of the sphere
    [[nodiscard]] glm::vec3 getLocation() const;
    /// \desc sets the x coordinate of the sphere's location
    /// \param x x coordinate in world space to position sphere at
    void setLocationX( GLfloat x );
    /// \desc sets the z coordinate of the sphere's location
    /// \param z z coordinate in world space to position sphere at
    void setLocationZ( GLfloat z );
    /// \desc gets the direction the sphere is moving
    /// \returns direction the sphere is moving
    /// \note vector will be normalized
    [[nodiscard]] glm::vec3 getDirection() const;
    /// \desc sets direction for the sphere to move
    /// \param newDirection direction for the sphere to move
    /// \note normalizes the input direction
    void setDirection(glm::vec3 newDirection);

    /// \desc renders the sphere at corresponding location, orientation, and size
    /// \param shaderProgram shader program to render the sphere against
    /// \param mvpUniformLocation uniform shader location for the MVP Matrix
    /// \param colorUniformLocation uniform shader location for the Color Tint
    /// \param modelMtx model matrix containing any transformations already applied
    /// \param projViewMtx precomputed ViewProjection matrix
    void draw( glm::mat4 modelMtx, glm::mat4 projMtx, glm::mat4 viewMtx );
    /// \desc rolls the sphere forward one unit along its direction
    void moveForward();
    /// \desc rolls the sphere backward one unit along its direction
    void moveBackward();

private:

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

        GLint viewMtx;
    } _shaderProgramUniformLocations;

    /// \desc the current rotation angle of the sphere
    GLfloat _rotation;
    /// \desc the location of the sphere in world coordinates
    glm::vec3 _location;
    /// \desc the direction of the sphere movement in world coordinates
    glm::vec3 _direction;

    /// \desc generates a random value within the range [0.5f, 1.0f]
    /// \returns random value within the range [0.5f, 1.0f]
    static GLfloat _genRandColor();
};

#endif // LAB11_MARBLE_H
