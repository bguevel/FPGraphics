#include "Lab02Engine.h"

#include <CSCI441/SimpleShader.hpp>
#include <CSCI441/objects.hpp>
#include <CSCI441/ArcballCam.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifndef M_PI
#define M_PI 3.14159265f
#endif

//*************************************************************************************
//
// Helper Functions

/**
 * @brief Simple helper function to return a random number between 0.0f and 1.0f
 * @note granularity and rand normalization is dependent on the value of RAND_MAX
 */
GLfloat getRand() {
    return (GLfloat)rand() / (GLfloat)RAND_MAX;
}

//*************************************************************************************
//
// Engine Setup

Lab02Engine::Lab02Engine()
      : CSCI441::OpenGLEngine(4, 1, 640, 640, "FP - The Grey Havens") {
    _gridVAO = 0;
    _numGridPoints = 0;
    _gridColor = glm::vec3(1.0f, 1.0f, 1.0f);
    _mousePosition = glm::vec2(MOUSE_UNINITIALIZED, MOUSE_UNINITIALIZED );
    _leftMouseButtonState = GLFW_RELEASE;
    for(auto& _key : _keys) _key = GL_FALSE;
    _cams = new CSCI441::Camera*[numCams];
}

Lab02Engine::~Lab02Engine() {
    for (int i = 0; i < numCams; ++i) {
        delete _cams[i];
    }
    delete[] _cams;
}

void Lab02Engine::mSetupGLFW() {
    CSCI441::OpenGLEngine::mSetupGLFW();                                // set up our OpenGL context

    glfwSetKeyCallback( mpWindow, lab02_engine_keyboard_callback );             // set our keyboard callback function
    glfwSetCursorPosCallback( mpWindow, lab02_engine_cursor_callback );         // set our cursor position callback function
    glfwSetMouseButtonCallback( mpWindow, lab02_engine_mouse_button_callback ); // set our mouse button callback function
}

void Lab02Engine::mSetupOpenGL() {
    glEnable( GL_DEPTH_TEST );					                        // enable depth testing
    glDepthFunc( GL_LESS );							                // use less than depth test

    glEnable(GL_BLEND);									            // enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	    // use one minus blending equation

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );	        // clear the frame buffer to black
}

void Lab02Engine::mSetupShaders() {
    _lightingShaderProgram = new CSCI441::ShaderProgram("shaders/lab05.v.glsl", "shaders/lab05.f.glsl" );
    _lightingShaderUniformLocations.mvpMatrix      = _lightingShaderProgram->getUniformLocation("mvpMatrix");
    _lightingShaderUniformLocations.materialDiffuse = _lightingShaderProgram->getUniformLocation("materialDiffuse");
    _lightingShaderUniformLocations.materialSpecular = _lightingShaderProgram->getUniformLocation("materialSpecular");
    _lightingShaderUniformLocations.materialShine = _lightingShaderProgram->getUniformLocation("materialShine");
    _lightingShaderUniformLocations.isEmitter = _lightingShaderProgram->getUniformLocation("isEmitter");
    // TODO #3A: assign uniforms
    _lightingShaderUniformLocations.lightColor = _lightingShaderProgram->getUniformLocation("light_color");
    _lightingShaderUniformLocations.lightPosition = _lightingShaderProgram->getUniformLocation("light_direction");
    _lightingShaderAttributeLocations.vPos         = _lightingShaderProgram->getAttributeLocation("vPos");
    _lightingShaderUniformLocations.vNormalMatrix = _lightingShaderProgram->getUniformLocation("mtxNormal");

    _lightingShaderUniformLocations.modelMatrix    = _lightingShaderProgram->getUniformLocation( "modelMatrix" );
    _lightingShaderUniformLocations.camPosition = _lightingShaderProgram->getUniformLocation("cameraPosition");

    _lightingShaderUniformLocations.spotLightCutoff      = _lightingShaderProgram->getUniformLocation( "spotLightCutoff" );
    _lightingShaderUniformLocations.spotLightDirection   = _lightingShaderProgram->getUniformLocation( "spotLightDirection" );
    _lightingShaderUniformLocations.spotLightPosition    = _lightingShaderProgram->getUniformLocation( "spotLightPosition" );
    _lightingShaderUniformLocations.spotLightOuterCutoff = _lightingShaderProgram->getUniformLocation( "spotLightOuterCutoff" );
    // TODO #3B: assign attributes
    _lightingShaderAttributeLocations.vNorm = _lightingShaderProgram->getAttributeLocation("vertexnorm");
    CSCI441::setVertexAttributeLocations(_lightingShaderAttributeLocations.vPos,_lightingShaderAttributeLocations.vNorm);

    _terrainShaderProgram = new CSCI441::ShaderProgram("shaders/height.v.glsl", "shaders/height.f.glsl");

    // Get uniform locations for terrain shader
    _terrainShaderUniformLocations.mvpMatrix           = _terrainShaderProgram->getUniformLocation("mvpMatrix");
    _terrainShaderUniformLocations.modelMatrix         = _terrainShaderProgram->getUniformLocation("modelMatrix");
    _terrainShaderUniformLocations.normalMatrix        = _terrainShaderProgram->getUniformLocation("normalMatrix");
    _terrainShaderUniformLocations.heightMap           = _terrainShaderProgram->getUniformLocation("heightMap");
    _terrainShaderUniformLocations.maxHeight           = _terrainShaderProgram->getUniformLocation("maxHeight");
    _terrainShaderUniformLocations.materialColor       = _terrainShaderProgram->getUniformLocation("materialColor");
    _terrainShaderUniformLocations.camPosition         = _terrainShaderProgram->getUniformLocation("cameraPosition");
    _terrainShaderUniformLocations.lightColor          = _terrainShaderProgram->getUniformLocation("light_color");
    _terrainShaderUniformLocations.lightPosition       = _terrainShaderProgram->getUniformLocation("light_direction");
    _terrainShaderUniformLocations.spotLightPosition   = _terrainShaderProgram->getUniformLocation("spotLightPosition");
    _terrainShaderUniformLocations.spotLightDirection  = _terrainShaderProgram->getUniformLocation("spotLightDirection");
    _terrainShaderUniformLocations.spotLightCutoff     = _terrainShaderProgram->getUniformLocation("spotLightCutoff");
    _terrainShaderUniformLocations.spotLightOuterCutoff= _terrainShaderProgram->getUniformLocation("spotLightOuterCutoff");
    _terrainShaderUniformLocations.texelSize = _terrainShaderProgram->getUniformLocation("texelSize");
    // Terrain Shader Attributes
    _terrainShaderAttributeLocations.vPos      = _terrainShaderProgram->getAttributeLocation("vPos");
    _terrainShaderAttributeLocations.vNormal   = _terrainShaderProgram->getAttributeLocation("vNormal");
    _terrainShaderAttributeLocations.vTexCoord = _terrainShaderProgram->getAttributeLocation("vTexCoord");

    _skyboxShaderProgram = new CSCI441::ShaderProgram( "shaders/skybox.v.glsl", "shaders/skybox.f.glsl" );

    // Get uniform locations
    _skyboxShaderUniformLocations.modelMatrix      = _skyboxShaderProgram->getUniformLocation( "model" );
    _skyboxShaderUniformLocations.viewMatrix       = _skyboxShaderProgram->getUniformLocation( "view" );
    _skyboxShaderUniformLocations.projectionMatrix = _skyboxShaderProgram->getUniformLocation( "projection" );
    _skyboxShaderUniformLocations.skyboxTexture    = _skyboxShaderProgram->getUniformLocation( "skyboxTexture" );
}

void Lab02Engine::mSetupBuffers() {
    maxHeight = 10.0f;
    // Initialize the plane (assuming this is part of your scene)

    _pPlayerCar = new Car(_lightingShaderProgram->getShaderProgramHandle(),
                        _lightingShaderUniformLocations.mvpMatrix,
                        _lightingShaderAttributeLocations.vNorm,
                        _lightingShaderUniformLocations.materialDiffuse,
                        _lightingShaderUniformLocations.materialSpecular,
                        _lightingShaderUniformLocations.materialShine,
                        _lightingShaderUniformLocations.isEmitter);
    // Load the height map first
    if(!loadHeightMap("heightmap.png")) { // Ensure "heightmap.png" is in the correct directory
        exit(EXIT_FAILURE);
    }
    _skyTex = _loadAndRegisterSkyboxTexture( "cubeMapFrozen.jpg" );
    // Now create the ground buffers using the loaded height map data
    _createGroundBuffers();

    // Generate other environment elements
    _generateEnvironment();

    _createSkyboxBuffers( );
}

void Lab02Engine::mSetupScene() {
    //_pFreeCam = new FreeCam();
    _cameraSpeed = glm::vec2(0.25f, 0.02f);

    _cams[CAM_ID::ARC_CAM] = new CSCI441::ArcballCam();
    _cams[CAM_ID::ARC_CAM]->setLookAtPoint(_pPlayerCar->getPosition());
    _cams[CAM_ID::ARC_CAM]->setRadius(15);
    _cams[CAM_ID::ARC_CAM]->setTheta(-M_PI / 3.0f );
    _cams[CAM_ID::ARC_CAM]->setPhi(8*M_PI/6);
    _cams[CAM_ID::ARC_CAM]->recomputeOrientation();

    _cams[CAM_ID::FIXED_CAM] = new CSCI441::ArcballCam();
    _cams[CAM_ID::FIXED_CAM]->setLookAtPoint(_pPlayerCar->getPosition());
    _cams[CAM_ID::FIXED_CAM]->setRadius(15);
    _cams[CAM_ID::FIXED_CAM]->setTheta(M_PI/2);
    _cams[CAM_ID::FIXED_CAM]->setPhi(8*M_PI/6);
    _cams[CAM_ID::FIXED_CAM]->recomputeOrientation();

    camID = CAM_ID::FIXED_CAM;

    // TODO #6: set lighting uniforms
    glm::vec3 lightDirection = glm::vec3(-1.0f, -1.0f, -1.0f);
    glm::vec3 lightColor = glm::vec3(0.2f, 0.2f, 0.2f);
    glProgramUniform3fv( _lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightColor, 1, glm::value_ptr(lightColor));
    glProgramUniform3fv( _lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightPosition, 1, glm::value_ptr(lightDirection));

    //******************************************************************
    glm::vec3 playerPosition = _pPlayerCar->getPosition();
    glm::vec3 spotLightPosition = glm::vec3(-110.0f,30.0f,-110.0f); // Spotlight above the Being
    glm::vec3 spotLightDirection = glm::vec3(0.0f,-1.0f,0.0f);
    GLfloat spotLightCutoff      = glm::cos( glm::radians( 40.0f ) );
    GLfloat spotLightOuterCutoff = glm::cos( glm::radians( 70.0f ) );

    glProgramUniform3fv( _lightingShaderProgram->getShaderProgramHandle( ), _lightingShaderUniformLocations.spotLightPosition, 1, glm::value_ptr( spotLightPosition ) );

    glProgramUniform3fv( _lightingShaderProgram->getShaderProgramHandle( ), _lightingShaderUniformLocations.spotLightDirection, 1, glm::value_ptr( spotLightDirection ) );

    glProgramUniform1f( _lightingShaderProgram->getShaderProgramHandle( ), _lightingShaderUniformLocations.spotLightOuterCutoff, spotLightOuterCutoff );

    glProgramUniform1f( _lightingShaderProgram->getShaderProgramHandle( ), _lightingShaderUniformLocations.spotLightCutoff, spotLightCutoff );

    // Pass lighting data to terrain shader
    glProgramUniform3fv(_terrainShaderProgram->getShaderProgramHandle(), _terrainShaderUniformLocations.lightPosition, 1, glm::value_ptr(lightDirection));
    glProgramUniform3fv(_terrainShaderProgram->getShaderProgramHandle(), _terrainShaderUniformLocations.lightColor, 1, glm::value_ptr(lightColor));
    glProgramUniform3fv(_terrainShaderProgram->getShaderProgramHandle(), _terrainShaderUniformLocations.camPosition, 1, glm::value_ptr(_cams[camID]->getPosition()));
    // Pass spotlight data
    glProgramUniform3fv(_terrainShaderProgram->getShaderProgramHandle(), _terrainShaderUniformLocations.spotLightPosition, 1, glm::value_ptr(spotLightPosition));
    glProgramUniform3fv(_terrainShaderProgram->getShaderProgramHandle(), _terrainShaderUniformLocations.spotLightDirection, 1, glm::value_ptr(spotLightDirection));
    glProgramUniform1f(_terrainShaderProgram->getShaderProgramHandle(), _terrainShaderUniformLocations.spotLightOuterCutoff, spotLightOuterCutoff);
    glProgramUniform1f(_terrainShaderProgram->getShaderProgramHandle(), _terrainShaderUniformLocations.spotLightCutoff, spotLightCutoff);

    printf("Directional Light Color: (%f, %f, %f)\n", lightColor.x, lightColor.y,lightColor.z);
    printf("Directional Light Direction: (%f, %f, %f)\n", lightDirection.x, lightDirection.y, lightDirection.z);
    printf("Spotlight Position: (%f, %f, %f)\n", spotLightPosition.x, spotLightPosition.y, spotLightPosition.z);
    printf("Spotlight Direction: (%f, %f, %f)\n", spotLightDirection.x, spotLightDirection.y, spotLightDirection.z);
    printf("Spotlight Cutoffs: Inner %f, Outer %f\n", spotLightCutoff, spotLightOuterCutoff);

}

bool Lab02Engine::loadHeightMap(const std::string& filepath) {
    unsigned char* data = stbi_load(filepath.c_str(), &heightMapWidth, &heightMapHeight, &heightMapChannels, 1); // Load as grayscale
    if (!data) {
        fprintf(stderr, "Failed to load height map: %s\n", filepath.c_str());
        return false;
    }

    // Create OpenGL texture
    glGenTextures(1, &_heightMapTextureID);
    glBindTexture(GL_TEXTURE_2D, _heightMapTextureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, heightMapWidth, heightMapHeight, 0, GL_RED, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Populate heightData for CPU-side height queries
    heightData.resize(heightMapWidth * heightMapHeight);
    for(int i = 0; i < heightMapWidth * heightMapHeight; ++i){
        heightData[i] = static_cast<float>(data[i]) / 255.0f * maxHeight;
    }

    stbi_image_free(data);
    return true;
}

float Lab02Engine::getTerrainHeight(float x, float z) const {
    // Map world coordinates to heightmap coordinates
    float terrainX = x + _centerX;
    float terrainZ = z + _centerZ;

    // Clamp the coordinates to the height map boundaries
    if (terrainX < 0.0f) terrainX = 0.0f;
    if (terrainZ < 0.0f) terrainZ = 0.0f;
    if (terrainX >= heightMapWidth - 1) terrainX = heightMapWidth - 1.001f; // Prevent overflow
    if (terrainZ >= heightMapHeight - 1) terrainZ = heightMapHeight - 1.001f;

    // Get the integer and fractional parts
    int x0 = static_cast<int>(floor(terrainX));
    int z0 = static_cast<int>(floor(terrainZ));
    float tx = terrainX - x0;
    float tz = terrainZ - z0;

    // Retrieve heights from the four surrounding vertices
    float h00 = heightData[z0 * heightMapWidth + x0];
    float h10 = heightData[z0 * heightMapWidth + (x0 + 1)];
    float h01 = heightData[(z0 + 1) * heightMapWidth + x0];
    float h11 = heightData[(z0 + 1) * heightMapWidth + (x0 + 1)];

    // Bilinear interpolation
    float h0 = h00 * (1 - tx) + h10 * tx;
    float h1 = h01 * (1 - tx) + h11 * tx;
    float interpolatedHeight = h0 * (1 - tz) + h1 * tz;

    return interpolatedHeight;
}

void Lab02Engine::_generateEnvironment() {
    //******************************************************************
    // parameters to make up our grid size and spacing, feel free to
    // play around with this
    const GLfloat GRID_WIDTH = WORLD_SIZE * 1.8f;
    const GLfloat GRID_LENGTH = WORLD_SIZE * 1.8f;
    const GLfloat GRID_SPACING_WIDTH = 1.0f;
    const GLfloat GRID_SPACING_LENGTH = 1.0f;
    // precomputed parameters based on above
    const GLfloat LEFT_END_POINT = -GRID_WIDTH / 2.0f - 5.0f;
    const GLfloat RIGHT_END_POINT = GRID_WIDTH / 2.0f + 5.0f;
    const GLfloat BOTTOM_END_POINT = -GRID_LENGTH / 2.0f - 5.0f;
    const GLfloat TOP_END_POINT = GRID_LENGTH / 2.0f + 5.0f;
    //******************************************************************
    // seed our RNG
    srand( time(nullptr) );                                                // seed our RNG


    // psych! everything's on a grid.




    //******************************************************************
    // draws a grid as our ground plane
    // do not edit this next section

    _gridColor = glm::vec3(1.0f, 1.0f, 1.0f);
    //******************************************************************
}

//*************************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!
void Lab02Engine::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // For lighting shader
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.modelMatrix, modelMtx);
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.mvpMatrix, mvpMtx);
    glm::mat3 normalMtx = glm::mat3(glm::transpose(glm::inverse(modelMtx)));
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.vNormalMatrix, normalMtx);
    // Set camera position uniform
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.camPosition, _cams[camID]->getPosition());

    // For terrain shader
    _terrainShaderProgram->setProgramUniform(_terrainShaderUniformLocations.mvpMatrix, mvpMtx);
    _terrainShaderProgram->setProgramUniform(_terrainShaderUniformLocations.modelMatrix, modelMtx);
    _terrainShaderProgram->setProgramUniform(_terrainShaderUniformLocations.normalMatrix, normalMtx);
    _terrainShaderProgram->setProgramUniform(_terrainShaderUniformLocations.camPosition, _cams[camID]->getPosition());
}

void Lab02Engine::_renderScene(glm::mat4 viewMtx, glm::mat4 projMtx) const {


    // Save current depth function and depth mask state
    GLint prevDepthFunc;
    glGetIntegerv( GL_DEPTH_FUNC, &prevDepthFunc );
    GLboolean prevDepthMask;
    glGetBooleanv( GL_DEPTH_WRITEMASK, &prevDepthMask );

    // First, render the skybox
    glDepthFunc( GL_LEQUAL ); // Change depth function for skybox
    glDepthMask( GL_FALSE );  // Disable depth writing

    _skyboxShaderProgram->useProgram( );

    // Remove translation from the view matrix
    glm::mat4 view = glm::mat4( glm::mat3( viewMtx ) );

    // Set shader uniforms
    _skyboxShaderProgram->setProgramUniform( _skyboxShaderUniformLocations.modelMatrix, glm::mat4( 1.0f ) );
    _skyboxShaderProgram->setProgramUniform( _skyboxShaderUniformLocations.viewMatrix, view );
    _skyboxShaderProgram->setProgramUniform( _skyboxShaderUniformLocations.projectionMatrix, projMtx );

    // Bind the skybox texture
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, _skyTex );
    _skyboxShaderProgram->setProgramUniform( _skyboxShaderUniformLocations.skyboxTexture, 0 );

    // Render the skybox cube
    glBindVertexArray( _skyboxVAO );
    glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0 );
    glBindVertexArray( 0 );

    // Restore depth function and depth mask
    glDepthFunc( prevDepthFunc );
    glDepthMask( prevDepthMask );

    // 1. Draw Terrain with Terrain Shader
    _terrainShaderProgram->useProgram();

    // Bind heightmap texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _heightMapTextureID);
    _terrainShaderProgram->setProgramUniform(_terrainShaderUniformLocations.heightMap, 0);
    glm::vec2 texelSize(1.0f / static_cast<float>(heightMapWidth),
                    1.0f / static_cast<float>(heightMapHeight));
    _terrainShaderProgram->setProgramUniform(_terrainShaderUniformLocations.texelSize, texelSize);

    // Set uniforms
    _terrainShaderProgram->setProgramUniform(_terrainShaderUniformLocations.maxHeight, maxHeight);

    // Set material color
    glm::vec3 terrainColor(0.5f, 0.8f, 0.2f); // The color of the terrain
    _terrainShaderProgram->setProgramUniform(_terrainShaderUniformLocations.materialColor, terrainColor);

    // Set matrices
    glm::mat4 modelMtxTerrain = glm::mat4(1.0f);
    _computeAndSendMatrixUniforms(modelMtxTerrain, viewMtx, projMtx);

    // Draw the terrain
    glBindVertexArray(_terrainVAO);
    glDrawElements(GL_TRIANGLES, terrainIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // 2. Draw Plane with Lighting Shader
    _lightingShaderProgram->useProgram();

    // Compute and send matrix uniforms for lighting shader
    glm::mat4 modelMtxPlane = glm::mat4(1.0f);
    glm::vec3 playerPos = _pPlayerCar->getPosition();
    //fprintf(stdout,"Player Position: (%f, %f, %f)\n", playerPos.x, playerPos.y, playerPos.z);
    //fprintf(stdout,"Player Position: (%f, %f, %f)\n", playerPos.x, playerPos.y, playerPos.z);
    modelMtxPlane = glm::translate(modelMtxPlane, _pPlayerCar->getPosition());
    _computeAndSendMatrixUniforms(modelMtxPlane, viewMtx, projMtx);

    // Draw the plane
    _pPlayerCar->draw(modelMtxPlane, viewMtx, projMtx);

}

void Lab02Engine::_updateScene(){

    glm::vec3 playerPos = _pPlayerCar->getPosition();

    // Retrieve the terrain height at the plane's current X and Z
    float terrainHeight = getTerrainHeight(playerPos.x, playerPos.z);

    // Define an offset to keep the plane above the terrain
    float heightOffset = 0.5f; // Adjust as needed for your simulation
    //printf("Terrain Height: %f\n", terrainHeight);
    //printf("Plane Height: %f\n", planePos.y);
    // Ensure the plane stays above the terrain
    if (playerPos.y < terrainHeight + heightOffset) {

        playerPos.y = terrainHeight + heightOffset;
        _pPlayerCar->setPosition(playerPos);
    }
    else if(playerPos.y>terrainHeight + heightOffset) {
        playerPos.y = terrainHeight + heightOffset;
        _pPlayerCar->setPosition(playerPos);
    }
    //keeping the original functionality with moving the cam
    if(_keys[GLFW_KEY_1]) {
        //Switch to Fixed
        camID = CAM_ID::FIXED_CAM;
    }
    else if (_keys[GLFW_KEY_2]) {
        //Switch to arc
        camID = CAM_ID::ARC_CAM;
    }
    if(_keys[GLFW_KEY_W] ) {
        if(!(_pPlayerCar->getPosition().x +0.2f < 100.0f && _pPlayerCar->getPosition().z +0.2f < 100.0f && _pPlayerCar->getPosition().x +0.2f > -100.0f && _pPlayerCar->getPosition().z +0.2f > -100.0f)) { // bounds checking, so that we can stay within the created world
            _pPlayerCar->_isFalling = true;
            //outofbounds = true;
        }
        //_pPlayerCar->setPosition(_pPlayerCar->getPosition() + (_pPlayerCar->getForwardDirection()*1.2f));
        _pPlayerCar->moveForward(1.0f);
        _pPlayerCar->setForwardDirection();
    }
    else if(_keys[GLFW_KEY_S] ) {
        if(!(_pPlayerCar->getPosition().x +0.2f < 100.0f && _pPlayerCar->getPosition().z +0.2f < 100.0f && _pPlayerCar->getPosition().x +0.2f > -100.0f && _pPlayerCar->getPosition().z +0.2f > -100.0f)) { // bounds checking, so that we can stay within the created world
            _pPlayerCar->_isFalling = true;
            //outofbounds = true;
        }
        //_pPlayerCar->setPosition(_pPlayerCar->getPosition() - (_pPlayerCar->getForwardDirection()*1.2f));
        _pPlayerCar->moveBackward(1.0f);
        _pPlayerCar->setForwardDirection();
    }
    else {
        _pPlayerCar->notMoving();
    }
    if(_keys[GLFW_KEY_D] ) {
        if (_pPlayerCar->isMoving) {
            _pPlayerCar->rotateSelf(-0.1f); // give the axis of travel and whether the axis involves the A key as then we need to inverse the angle
            _pPlayerCar->setForwardDirection();
            _cams[CAM_ID::FIXED_CAM]->setTheta(_cams[CAM_ID::FIXED_CAM]->getTheta() + 0.1f);

        }
        _pPlayerCar->isTurnRight = true;

    }
    else {
        _pPlayerCar->isTurnRight = false;
    }
    if(_keys[GLFW_KEY_A] ) {
        if (_pPlayerCar->isMoving) {
            _pPlayerCar->rotateSelf(0.1f); // give the axis of travel and whether the axis involves the A key as then we need to inverse the angle
            _pPlayerCar->setForwardDirection();
            _cams[CAM_ID::FIXED_CAM]->setTheta(_cams[CAM_ID::FIXED_CAM]->getTheta() - 0.1f);

        }
        _pPlayerCar->isTurnLeft = true;

    }
    else {
        _pPlayerCar->isTurnLeft = false;
    }
    _pPlayerCar->setForwardDirection();
    _cams[camID]->setLookAtPoint(_pPlayerCar->getPosition());
    _cams[camID]->recomputeOrientation();
    _pPlayerCar->update();
    _moveSpotlight();

}

void Lab02Engine::handleKeyEvent(GLint key, GLint action) {
    if(key != GLFW_KEY_UNKNOWN)
        _keys[key] = ((action == GLFW_PRESS) || (action == GLFW_REPEAT));

    if(action == GLFW_PRESS) {
        switch( key ) {
            // quit!
            case GLFW_KEY_Q:
            case GLFW_KEY_ESCAPE:
                setWindowShouldClose();
            break;

            default: break; // suppress CLion warning
        }
    }
}

void Lab02Engine::handleMouseButtonEvent(GLint button, GLint action) {
    // if the event is for the left mouse button
    if( button == GLFW_MOUSE_BUTTON_LEFT ) {
        // update the left mouse button's state
        _leftMouseButtonState = action;
    }
}

void Lab02Engine::handleCursorPositionEvent(glm::vec2 currMousePosition) {
    // if mouse hasn't moved in the window, prevent camera from flipping out
    if(_mousePosition.x == MOUSE_UNINITIALIZED) {
        _mousePosition = currMousePosition;
    }

    // if the left mouse button is being held down while the mouse is moving
    if(_leftMouseButtonState == GLFW_PRESS) {
        if(_keys[GLFW_KEY_LEFT_SHIFT] || _keys[GLFW_KEY_RIGHT_SHIFT]) { // this is to check if shift is being pressed so we can zoom
            if(_mousePosition.y - currMousePosition.y < 0) { // zoom in or out
                _cams[camID]->moveBackward(_cameraSpeed.x);
            }
            else {
                _cams[camID]->moveForward(_cameraSpeed.x);
            }
        }
        else {
            // rotate the camera by the distance the mouse moved
            if (camID != CAM_ID::FIXED_CAM) _cams[camID]->rotate((currMousePosition.x - _mousePosition.x) * 0.005f,
                             (_mousePosition.y - currMousePosition.y) * 0.005f );
        }
    }

    // update the mouse position
    _mousePosition = currMousePosition;
}
GLuint Lab02Engine::_loadAndRegisterSkyboxTexture( const char* FILENAME )
{
    // Our handle to the GPU texture
    GLuint textureHandle = 0;

    // Disable vertical flipping to load the image as-is
    stbi_set_flip_vertically_on_load( true );

    // Will hold image parameters after load
    GLint imageWidth, imageHeight, imageChannels;

    // Load image from file
    GLubyte* data = stbi_load( FILENAME, &imageWidth, &imageHeight, &imageChannels, 0 );

    // Check if data was read from file
    if ( data )
    {
        // Determine the storage format
        GLint STORAGE_TYPE = ( imageChannels == 4 ? GL_RGBA : GL_RGB );

        // Generate a texture handle
        glGenTextures( 1, &textureHandle );
        // Bind it to be active
        glBindTexture( GL_TEXTURE_2D, textureHandle );

        // Set texture parameters
        // Set texture filtering
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        // Set texture wrapping to clamp to edge
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE ); // Wrap horizontally
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE ); // Wrap vertically

        // Transfer image data to the GPU
        glTexImage2D( GL_TEXTURE_2D, 0, STORAGE_TYPE, imageWidth, imageHeight, 0, STORAGE_TYPE, GL_UNSIGNED_BYTE, data );

        // Print out image info for debugging
        fprintf( stdout, "[INFO]: Skybox texture loaded: %s (Width: %d, Height: %d, Channels: %d)\n", FILENAME, imageWidth, imageHeight, imageChannels );

        // Release image memory from CPU - it now lives on the GPU
        stbi_image_free( data );
    }
    else
    {
        // Load failed
        fprintf( stderr, "[ERROR]: Could not load skybox texture \"%s\"\n", FILENAME );
    }

    // Return generated texture handle
    return textureHandle;
}
void Lab02Engine::_createGroundBuffers() {
    // Define scaling factors
    float scaleX = 1.0f; // Adjust as needed
    float scaleZ = 1.0f; // Adjust as needed

    _centerX = (heightMapWidth - 1) * scaleX / 2.0f;
    _centerZ = (heightMapHeight - 1) * scaleZ / 2.0f;

    // Generate vertices
    for (int z = 0; z < heightMapHeight; ++z) {
        for (int x = 0; x < heightMapWidth; ++x) {
            float y = 0.0f; // Height will be computed in the vertex shader
            float posX = x * scaleX - _centerX;
            float posZ = z * scaleZ - _centerZ;
            terrainVertices.push_back(posX); // X
            terrainVertices.push_back(y);     // Y
            terrainVertices.push_back(posZ); // Z

            // Placeholder normal
            terrainVertices.push_back(0.0f); // Normal X
            terrainVertices.push_back(1.0f); // Normal Y
            terrainVertices.push_back(0.0f); // Normal Z

            // Texture coordinates
            float u = (float)x / (heightMapWidth - 1);
            float v = (float)z / (heightMapHeight - 1);
            terrainVertices.push_back(u);
            terrainVertices.push_back(v);
        }
    }

    // Generate indices for triangles, excluding quads within the hole radius
    for(int z = 0; z < heightMapHeight - 1; ++z){
        for(int x = 0; x < heightMapWidth - 1; ++x){
            // Calculate positions for current quad
            float posX0 = x * scaleX - _centerX;
            float posZ0 = z * scaleZ - _centerZ;
            float posX1 = (x + 1) * scaleX - _centerX;
            float posZ1 = (z + 1) * scaleZ - _centerZ;

            // Compute distance from center for the quad's center
            float centerQuadX = (posX0 + posX1) / 2.0f;
            float centerQuadZ = (posZ0 + posZ1) / 2.0f;
            float distQuadCenter = sqrt(centerQuadX * centerQuadX + centerQuadZ * centerQuadZ);

            // Skip quads within the hole radius
            if(distQuadCenter < 90.0){
                continue; // Do not generate indices for this quad, creating a hole
            }

            // Generate indices for the quad
            int topLeft = z * heightMapWidth + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * heightMapWidth + x;
            int bottomRight = bottomLeft + 1;

            // First triangle
            terrainIndices.push_back(topLeft);
            terrainIndices.push_back(bottomLeft);
            terrainIndices.push_back(topRight);

            // Second triangle
            terrainIndices.push_back(topRight);
            terrainIndices.push_back(bottomLeft);
            terrainIndices.push_back(bottomRight);
        }
    }

    // Calculate normals
    calculateTerrainNormals();

    // Create and upload buffers
    glGenVertexArrays(1, &_terrainVAO);
    glBindVertexArray(_terrainVAO);

    glGenBuffers(1, &_terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, _terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, terrainVertices.size() * sizeof(float), terrainVertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &_terrainIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _terrainIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, terrainIndices.size() * sizeof(unsigned int), terrainIndices.data(), GL_STATIC_DRAW);

    // Set attribute pointers while VAO is bound
    const GLsizei stride = 8 * sizeof(float); // 3 pos + 3 normal + 2 texCoord

    // Position attribute
    glEnableVertexAttribArray(_terrainShaderAttributeLocations.vPos);
    glVertexAttribPointer(
        _terrainShaderAttributeLocations.vPos,
        3,              // size (x, y, z)
        GL_FLOAT,       // type
        GL_FALSE,       // normalized?
        stride,         // stride
        (void*)0        // array buffer offset
    );

    // Normal attribute
    glEnableVertexAttribArray(_terrainShaderAttributeLocations.vNormal);
    glVertexAttribPointer(
        _terrainShaderAttributeLocations.vNormal,
        3,              // size (nx, ny, nz)
        GL_FLOAT,
        GL_FALSE,
        stride,
        (void*)(3 * sizeof(float)) // Offset after positions (3 floats)
    );

    // Texture Coordinate attribute
    glEnableVertexAttribArray(_terrainShaderAttributeLocations.vTexCoord);
    glVertexAttribPointer(
        _terrainShaderAttributeLocations.vTexCoord,
        2,              // size (u, v)
        GL_FLOAT,
        GL_FALSE,
        stride,
        (void*)(6 * sizeof(float)) // Offset after positions and normals (6 floats)
    );

    glBindVertexArray(0);
}


void Lab02Engine::calculateTerrainNormals() {
    // Initialize normals array
    std::vector<glm::vec3> normals(heightMapWidth * heightMapHeight, glm::vec3(0.0f));

    // Iterate over each triangle in the terrain
    for (int z = 0; z < heightMapHeight - 1; ++z) {
        for (int x = 0; x < heightMapWidth - 1; ++x) {
            // Define vertices of the two triangles in the grid cell
            int topLeft = z * heightMapWidth + x;
            int topRight = z * heightMapWidth + (x + 1);
            int bottomLeft = (z + 1) * heightMapWidth + x;
            int bottomRight = (z + 1) * heightMapWidth + (x + 1);

            // First Triangle (topLeft, bottomLeft, topRight)
            glm::vec3 v0 = glm::vec3(terrainVertices[topLeft * 8 + 0],
                                     terrainVertices[topLeft * 8 + 1],
                                     terrainVertices[topLeft * 8 + 2]);
            glm::vec3 v1 = glm::vec3(terrainVertices[bottomLeft * 8 + 0],
                                     terrainVertices[bottomLeft * 8 + 1],
                                     terrainVertices[bottomLeft * 8 + 2]);
            glm::vec3 v2 = glm::vec3(terrainVertices[topRight * 8 + 0],
                                     terrainVertices[topRight * 8 + 1],
                                     terrainVertices[topRight * 8 + 2]);

            glm::vec3 edge1 = v1 - v0;
            glm::vec3 edge2 = v2 - v0;
            glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));

            // Accumulate normals for each vertex
            normals[topLeft] += faceNormal;
            normals[bottomLeft] += faceNormal;
            normals[topRight] += faceNormal;

            // Second Triangle (bottomLeft, bottomRight, topRight)
            glm::vec3 v3 = glm::vec3(terrainVertices[bottomRight * 8 + 0],
                                     terrainVertices[bottomRight * 8 + 1],
                                     terrainVertices[bottomRight * 8 + 2]);

            glm::vec3 edge3 = v3 - v1;
            glm::vec3 edge4 = v2 - v1;
            glm::vec3 faceNormal2 = glm::normalize(glm::cross(edge3, edge4));

            // Accumulate normals for each vertex
            normals[bottomLeft] += faceNormal2;
            normals[bottomRight] += faceNormal2;
            normals[topRight] += faceNormal2;
        }
    }

    // Normalize the accumulated normals
    for (int i = 0; i < normals.size(); ++i) {
        normals[i] = glm::normalize(normals[i]);
        // Corrected indexing: stride is 8 floats
        terrainVertices[i * 8 + 3] = normals[i].x;
        terrainVertices[i * 8 + 4] = normals[i].y;
        terrainVertices[i * 8 + 5] = normals[i].z;
    }

    // Update VBO with new normals
    glBindBuffer(GL_ARRAY_BUFFER, _terrainVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, terrainVertices.size() * sizeof(float), terrainVertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Lab02Engine::_moveSpotlight() {
    glm::vec3 spotLightPosition = _pPlayerCar->getPosition() + (_pPlayerCar->getForwardDirection()*5.0f); // Spotlight above the Being
    glm::vec3 spotLightDirection = _pPlayerCar->getForwardDirection();

    glProgramUniform3fv( _lightingShaderProgram->getShaderProgramHandle( ), _lightingShaderUniformLocations.spotLightPosition, 1, glm::value_ptr( spotLightPosition ) );
    glProgramUniform3fv( _lightingShaderProgram->getShaderProgramHandle( ), _lightingShaderUniformLocations.spotLightDirection, 1, glm::value_ptr( spotLightDirection ) );

    glProgramUniform3fv(_terrainShaderProgram->getShaderProgramHandle(), _terrainShaderUniformLocations.spotLightPosition, 1, glm::value_ptr(spotLightPosition));
    glProgramUniform3fv(_terrainShaderProgram->getShaderProgramHandle(), _terrainShaderUniformLocations.spotLightDirection, 1, glm::value_ptr(spotLightDirection));

}


void Lab02Engine::run() {
    printf("\nControls:\n");
    printf("\tW A S D - forwards, backwards, and side to side for the character in the world\n");
    printf("\tMouse Drag - Pan camera side to side and up and down\n");
    printf("\tMouse Drag up and down + shift - Zoom camera in and out\n");
    printf("\tQ / ESC - quit\n");


    //  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
    //	until the user decides to close the window and quit the program.  Without a loop, the
    //	window will display once and then the program exits.
    while( !glfwWindowShouldClose(mpWindow) ) {	// check if the window was instructed to be closed
        glDrawBuffer( GL_BACK );				// work with our back frame buffer
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// clear the current color contents and depth buffer in the window

        // Get the size of our framebuffer.  Ideally this should be the same dimensions as our window, but
        // when using a Retina display the actual window can be larger than the requested window.  Therefore,
        // query what the actual size of the window we are rendering to is.
        GLint framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize( mpWindow, &framebufferWidth, &framebufferHeight );

        // update the viewport - tell OpenGL we want to render to the whole window
        glViewport( 0, 0, framebufferWidth, framebufferHeight );

        // update the projection matrix based on the window size
        // the GL_PROJECTION matrix governs properties of the view coordinates;
        // i.e. what gets seen - use a perspective projection that ranges
        // with a FOV of 45 degrees, for our current aspect ratio, and Z ranges from [0.001, 1000].
        // set up our look at matrix to position our camera
        // multiply by the look at matrix - this is the same as our view matrix
        //CSCI441::SimpleShader3::setViewMatrix(viewMtx);

        _renderScene(_cams[camID]->getViewMatrix(), _cams[camID]->getProjectionMatrix());					// draw everything to the window
        _updateScene();
        glfwSwapBuffers(mpWindow);       // flush the OpenGL commands and make sure they get rendered!
        glfwPollEvents();				// check for any events and signal to redraw screen
    }
}
void Lab02Engine::_createSkyboxBuffers( )
{
  struct SkyboxVertex
  {
      glm::vec3 position;
      glm::vec2 texCoords;
  };

  const float texWidth  = 1.0f / 4.0f;
  const float texHeight = 1.0f / 3.0f;

  std::vector<SkyboxVertex> vertices = {
    // Right face (+X)
    {  glm::vec3( 1, -1, -1 ), glm::vec2( 2 * texWidth,     texHeight ) }, // Bottom-left
    {  glm::vec3( 1, -1,  1 ), glm::vec2( 3 * texWidth,     texHeight ) }, // Bottom-right
    {  glm::vec3( 1,  1,  1 ), glm::vec2( 3 * texWidth, 2 * texHeight ) }, // Top-right
    {  glm::vec3( 1,  1, -1 ), glm::vec2( 2 * texWidth, 2 * texHeight ) }, // Top-left

    // Left face (-X)
    { glm::vec3( -1, -1,  1 ), glm::vec2( 0 * texWidth,     texHeight ) }, // Bottom-left
    { glm::vec3( -1, -1, -1 ), glm::vec2( 1 * texWidth,     texHeight ) }, // Bottom-right
    { glm::vec3( -1,  1, -1 ), glm::vec2( 1 * texWidth, 2 * texHeight ) }, // Top-right
    { glm::vec3( -1,  1,  1 ), glm::vec2( 0 * texWidth, 2 * texHeight ) }, // Top-left

    // Top face (+Y)
    { glm::vec3( -1,  1, -1 ), glm::vec2( 1 * texWidth, 2 * texHeight ) }, // Bottom-left
    {  glm::vec3( 1,  1, -1 ), glm::vec2( 2 * texWidth, 2 * texHeight ) }, // Bottom-right
    {  glm::vec3( 1,  1,  1 ), glm::vec2( 2 * texWidth, 3 * texHeight ) }, // Top-right
    { glm::vec3( -1,  1,  1 ), glm::vec2( 1 * texWidth, 3 * texHeight ) }, // Top-left

    // Bottom face (-Y)
    { glm::vec3( -1, -1,  1 ), glm::vec2( 1 * texWidth, 0 * texHeight ) }, // Bottom-left
    {  glm::vec3( 1, -1,  1 ), glm::vec2( 2 * texWidth, 0 * texHeight ) }, // Bottom-right
    {  glm::vec3( 1, -1, -1 ), glm::vec2( 2 * texWidth, 1 * texHeight ) }, // Top-right
    { glm::vec3( -1, -1, -1 ), glm::vec2( 1 * texWidth, 1 * texHeight ) }, // Top-left

    // Front face (+Z)
    { glm::vec3( -1, -1, -1 ), glm::vec2( 1 * texWidth,     texHeight ) }, // Bottom-left
    {  glm::vec3( 1, -1, -1 ), glm::vec2( 2 * texWidth,     texHeight ) }, // Bottom-right
    {  glm::vec3( 1,  1, -1 ), glm::vec2( 2 * texWidth, 2 * texHeight ) }, // Top-right
    { glm::vec3( -1,  1, -1 ), glm::vec2( 1 * texWidth, 2 * texHeight ) }, // Top-left

    // Back face (-Z)
    {  glm::vec3( 1, -1,  1 ), glm::vec2( 3 * texWidth,     texHeight ) }, // Bottom-left
    { glm::vec3( -1, -1,  1 ), glm::vec2( 4 * texWidth,     texHeight ) }, // Bottom-right
    { glm::vec3( -1,  1,  1 ), glm::vec2( 4 * texWidth, 2 * texHeight ) }, // Top-right
    {  glm::vec3( 1,  1,  1 ), glm::vec2( 3 * texWidth, 2 * texHeight ) }, // Top-left
  };

  // Indices for drawing the cube with GL_TRIANGLES
  std::vector<GLuint> indices = {
    // Right face
    0,
    1,
    2,
    2,
    3,
    0,

    // Left face
    4,
    5,
    6,
    6,
    7,
    4,

    // Top face
    8,
    9,
    10,
    10,
    11,
    8,

    // Bottom face
    12,
    13,
    14,
    14,
    15,
    12,

    // Front face
    16,
    17,
    18,
    18,
    19,
    16,

    // Back face
    20,
    21,
    22,
    22,
    23,
    20,
  };

  // Create VAO and VBO
  glGenVertexArrays( 1, &_skyboxVAO );
  glGenBuffers( 1, &_skyboxVBO );
  GLuint skyboxEBO;
  glGenBuffers( 1, &skyboxEBO );

  glBindVertexArray( _skyboxVAO );

  glBindBuffer( GL_ARRAY_BUFFER, _skyboxVBO );
  glBufferData( GL_ARRAY_BUFFER, vertices.size( ) * sizeof( SkyboxVertex ), &vertices[0], GL_STATIC_DRAW );

  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, skyboxEBO );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size( ) * sizeof( GLuint ), &indices[0], GL_STATIC_DRAW );

  // Position attribute
  glEnableVertexAttribArray( 0 ); // Corresponds to location 0 in shader
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( SkyboxVertex ), (void*)0 );

  // Texture coordinate attribute
  glEnableVertexAttribArray( 1 ); // Corresponds to location 1 in shader
  glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof( SkyboxVertex ), (void*)offsetof( SkyboxVertex, texCoords ) );

  glBindVertexArray( 0 );
}

//*************************************************************************************
//
// Callbacks

void lab02_engine_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods ) {
    auto engine = (Lab02Engine*) glfwGetWindowUserPointer(window);
    // pass the key and action through to the engine
    engine->handleKeyEvent(key, action);
}

void lab02_engine_cursor_callback(GLFWwindow *window, double x, double y ) {
    auto engine = (Lab02Engine*) glfwGetWindowUserPointer(window);

    // pass the cursor position through to the engine
    engine->handleCursorPositionEvent(glm::vec2(x, y));
}

void lab02_engine_mouse_button_callback(GLFWwindow *window, int button, int action, int mods ) {
    auto engine = (Lab02Engine*) glfwGetWindowUserPointer(window);

    // pass the mouse button and action through to the engine
    engine->handleMouseButtonEvent(button, action);
}
