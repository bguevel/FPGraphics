#ifndef FP_TREE_H
#define FP_TREE_H

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "CSCI441/materials.hpp"

class Tree {
public:
  Tree(glm::vec3 position, GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation,  GLint materialDiffuseUniformLocation, GLint materialSpecularUniformLocation, GLint materialShineUniformLocation, GLint emitterUniformLocation);

  void draw(glm::mat4 viewMtx, glm::mat4 projMtx );


private:
  glm::vec3 position;

  void _sendMaterial(const CSCI441::Materials::Material&) const;
  void _computeAndSendMatrixUniforms( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;


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

};



#endif //TREE_H
