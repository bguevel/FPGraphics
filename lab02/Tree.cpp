#include "Tree.h"

#include <CSCI441/objects.hpp>
#include <CSCI441/SimpleShader.hpp>

Tree::Tree(glm::vec3 position, GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation,  GLint materialDiffuseUniformLocation, GLint materialSpecularUniformLocation, GLint materialShineUniformLocation, GLint emitterUniformLocation){
    this->position = position;
    _shaderProgramHandle                             = shaderProgramHandle;
    _shaderProgramUniformLocations.mvpMtx            = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx         = normalMtxUniformLocation;
    _shaderProgramUniformLocations.materialDiffuse   = materialDiffuseUniformLocation;
    _shaderProgramUniformLocations.materialSpecular  = materialSpecularUniformLocation;
    _shaderProgramUniformLocations.materialShine     = materialShineUniformLocation;
    _shaderProgramUniformLocations.isEmitter         = emitterUniformLocation;
}

void Tree::draw(glm::mat4 viewMtx, glm::mat4 projMtx) {
    glm::mat4 modelMtx = glm::translate(glm::mat4(1.0f), position);
    //Trunk
    _sendMaterial(CSCI441::Materials::BRONZE_POLISHED);
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    CSCI441::drawSolidCylinder(1.0f,1.0f,5,10,10);
    //Leaves
    _sendMaterial(CSCI441::Materials::CYAN_PLASTIC);
    glm::mat4 leaf = glm::translate(modelMtx, glm::vec3(0.0f,5.0f, 0.0f));
    _computeAndSendMatrixUniforms(leaf, viewMtx, projMtx);
    CSCI441::drawSolidCylinder(1.5f,0.0f,2,10,10);
    leaf = glm::translate(leaf, glm::vec3(0.0f, 1.0f, 0.0f));
    _computeAndSendMatrixUniforms(leaf, viewMtx, projMtx);
    CSCI441::drawSolidCylinder(1.5f,0.0f,2,10,10);
    leaf = glm::translate(leaf, glm::vec3(0.0f, 1.0f, 0.0f));
    _computeAndSendMatrixUniforms(leaf, viewMtx, projMtx);
    CSCI441::drawSolidCylinder(1.5f,0.0f,2,10,10);






}

void Tree::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex

    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.modelMtx, 1, GL_FALSE, glm::value_ptr(modelMtx) );

    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, glm::value_ptr(mvpMtx) );

    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, glm::value_ptr(normalMtx) );
}


void Tree::_sendMaterial(const CSCI441::Materials::Material &mat) const {
    glProgramUniform4fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialDiffuse, 1, glm::value_ptr(mat.getDiffuse()));
    glProgramUniform4fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialSpecular, 1, glm::value_ptr(mat.getSpecular()));
    glProgramUniform1f(_shaderProgramHandle, _shaderProgramUniformLocations.materialShine,mat.shininess);
}