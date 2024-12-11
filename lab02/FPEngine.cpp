#include "FPEngine.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//*************************************************************************************
//
// Helper Functions
GLfloat gen_rand_GLfloat( ) { return rand( ) / (GLfloat)RAND_MAX; }

//*************************************************************************************
//
// Engine Setup

FPEngine::FPEngine( )
    : CSCI441::OpenGLEngine( 4, 1, 640, 640, "FP - The Grey Havens" )
{
    _gridColor            = glm::vec3( 1.0f, 1.0f, 1.0f );
    _mousePosition        = glm::vec2( MOUSE_UNINITIALIZED, MOUSE_UNINITIALIZED );
    _leftMouseButtonState = GLFW_RELEASE;
    for ( auto& _key : _keys )
        _key = GL_FALSE;
    _cams = new CSCI441::Camera*[numCams];
}

FPEngine::~FPEngine( )
{
    for ( int i = 0; i < numCams; ++i )
    {
        delete _cams[i];
    }
    delete[] _cams;
}

void FPEngine::mSetupGLFW( )
{
    CSCI441::OpenGLEngine::mSetupGLFW( ); // set up our OpenGL context

    glfwSetKeyCallback( mpWindow, lab02_engine_keyboard_callback );             // set our keyboard callback function
    glfwSetCursorPosCallback( mpWindow, lab02_engine_cursor_callback );         // set our cursor position callback function
    glfwSetMouseButtonCallback( mpWindow, lab02_engine_mouse_button_callback ); // set our mouse button callback function
}

void FPEngine::mSetupOpenGL( )
{
    glEnable( GL_DEPTH_TEST ); // enable depth testing
    glDepthFunc( GL_LESS );    // use less than depth test

    glEnable( GL_BLEND );                                // enable blending
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); // use one minus blending equation

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f ); // clear the frame buffer to black
}

void FPEngine::mSetupShaders( )
{
    _lightingShaderProgram                           = new CSCI441::ShaderProgram( "shaders/fp.v.glsl", "shaders/fp.f.glsl" );
    _lightingShaderUniformLocations.mvpMatrix        = _lightingShaderProgram->getUniformLocation( "mvpMatrix" );
    _lightingShaderUniformLocations.materialDiffuse  = _lightingShaderProgram->getUniformLocation( "materialDiffuse" );
    _lightingShaderUniformLocations.materialSpecular = _lightingShaderProgram->getUniformLocation( "materialSpecular" );
    _lightingShaderUniformLocations.materialShine    = _lightingShaderProgram->getUniformLocation( "materialShine" );
    _lightingShaderUniformLocations.isEmitter        = _lightingShaderProgram->getUniformLocation( "isEmitter" );

    _lightingShaderUniformLocations.lightColor    = _lightingShaderProgram->getUniformLocation( "light_color" );
    _lightingShaderUniformLocations.lightPosition = _lightingShaderProgram->getUniformLocation( "light_direction" );
    _lightingShaderAttributeLocations.vPos        = _lightingShaderProgram->getAttributeLocation( "vPos" );
    _lightingShaderUniformLocations.vNormalMatrix = _lightingShaderProgram->getUniformLocation( "mtxNormal" );

    _lightingShaderUniformLocations.modelMatrix = _lightingShaderProgram->getUniformLocation( "modelMatrix" );
    _lightingShaderUniformLocations.camPosition = _lightingShaderProgram->getUniformLocation( "cameraPosition" );

    _lightingShaderUniformLocations.spotLightCutoff      = _lightingShaderProgram->getUniformLocation( "spotLightCutoff" );
    _lightingShaderUniformLocations.spotLightDirection   = _lightingShaderProgram->getUniformLocation( "spotLightDirection" );
    _lightingShaderUniformLocations.spotLightPosition    = _lightingShaderProgram->getUniformLocation( "spotLightPosition" );
    _lightingShaderUniformLocations.spotLightOuterCutoff = _lightingShaderProgram->getUniformLocation( "spotLightOuterCutoff" );

    _lightingShaderAttributeLocations.vNorm = _lightingShaderProgram->getAttributeLocation( "vertexnorm" );
    CSCI441::setVertexAttributeLocations( _lightingShaderAttributeLocations.vPos, _lightingShaderAttributeLocations.vNorm );

    _terrainShaderProgram = new CSCI441::ShaderProgram( "shaders/height.v.glsl", "shaders/height.f.glsl" );

    // Get uniform locations for terrain shader
    _terrainShaderUniformLocations.mvpMatrix            = _terrainShaderProgram->getUniformLocation( "mvpMatrix" );
    _terrainShaderUniformLocations.modelMatrix          = _terrainShaderProgram->getUniformLocation( "modelMatrix" );
    _terrainShaderUniformLocations.normalMatrix         = _terrainShaderProgram->getUniformLocation( "normalMatrix" );
    _terrainShaderUniformLocations.heightMap            = _terrainShaderProgram->getUniformLocation( "heightMap" );
    _terrainShaderUniformLocations.trackFilter          = _terrainShaderProgram->getUniformLocation( "trackFilter" );
    _terrainShaderUniformLocations.trackTexture         = _terrainShaderProgram->getUniformLocation( "trackTexture" );
    _terrainShaderUniformLocations.sceneTexture         = _terrainShaderProgram->getUniformLocation( "sceneTexture" );
    _terrainShaderUniformLocations.eggTexture           = _terrainShaderProgram->getUniformLocation( "eggTexture" );
    _terrainShaderUniformLocations.maxHeight            = _terrainShaderProgram->getUniformLocation( "maxHeight" );
    _terrainShaderUniformLocations.camPosition          = _terrainShaderProgram->getUniformLocation( "cameraPosition" );
    _terrainShaderUniformLocations.lightColor           = _terrainShaderProgram->getUniformLocation( "light_color" );
    _terrainShaderUniformLocations.lightPosition        = _terrainShaderProgram->getUniformLocation( "light_direction" );
    _terrainShaderUniformLocations.spotLightPosition    = _terrainShaderProgram->getUniformLocation( "spotLightPosition" );
    _terrainShaderUniformLocations.spotLightDirection   = _terrainShaderProgram->getUniformLocation( "spotLightDirection" );
    _terrainShaderUniformLocations.spotLightCutoff      = _terrainShaderProgram->getUniformLocation( "spotLightCutoff" );
    _terrainShaderUniformLocations.spotLightOuterCutoff = _terrainShaderProgram->getUniformLocation( "spotLightOuterCutoff" );
    _terrainShaderUniformLocations.texelSize            = _terrainShaderProgram->getUniformLocation( "texelSize" );
    _terrainShaderUniformLocations.isReverse            = _terrainShaderProgram->getUniformLocation( "isReverse" );
    // Terrain Shader Attributes
    _terrainShaderAttributeLocations.vPos      = _terrainShaderProgram->getAttributeLocation( "vPos" );
    _terrainShaderAttributeLocations.vNormal   = _terrainShaderProgram->getAttributeLocation( "vNormal" );
    _terrainShaderAttributeLocations.vTexCoord = _terrainShaderProgram->getAttributeLocation( "vTexCoord" );

    _skyboxShaderProgram = new CSCI441::ShaderProgram( "shaders/skybox.v.glsl", "shaders/skybox.f.glsl" );

    // Get uniform locations
    _skyboxShaderUniformLocations.modelMatrix      = _skyboxShaderProgram->getUniformLocation( "model" );
    _skyboxShaderUniformLocations.viewMatrix       = _skyboxShaderProgram->getUniformLocation( "view" );
    _skyboxShaderUniformLocations.projectionMatrix = _skyboxShaderProgram->getUniformLocation( "projection" );
    _skyboxShaderUniformLocations.skyboxTexture    = _skyboxShaderProgram->getUniformLocation( "skyboxTexture" );

    // Speed line shader program
    _speedLineShaderProgram = new CSCI441::ShaderProgram( "shaders/speedLines.v.glsl", "shaders/speedLines.g.glsl", "shaders/speedLines.f.glsl" );

    // Get uniform locations
    _speedLineUniformLocations.mvpMatrix     = _speedLineShaderProgram->getUniformLocation( "mvpMatrix" );
}

void FPEngine::mSetupBuffers( )
{
    maxHeight = 10.0f;

    _pPlayerCar = new Car( _lightingShaderProgram->getShaderProgramHandle( ),
                           _lightingShaderUniformLocations.mvpMatrix,
                           _lightingShaderAttributeLocations.vNorm,
                           _lightingShaderUniformLocations.materialDiffuse,
                           _lightingShaderUniformLocations.materialSpecular,
                           _lightingShaderUniformLocations.materialShine,
                           _lightingShaderUniformLocations.isEmitter,
                           false);

    _pAICar = new Car( _lightingShaderProgram->getShaderProgramHandle( ),
                       _lightingShaderUniformLocations.mvpMatrix,
                       _lightingShaderAttributeLocations.vNorm,
                       _lightingShaderUniformLocations.materialDiffuse,
                       _lightingShaderUniformLocations.materialSpecular,
                       _lightingShaderUniformLocations.materialShine,
                       _lightingShaderUniformLocations.isEmitter,
                       true);

    // Load the height map first
    if ( !loadHeightMap( "heightmap.png" ) )
    { // Ensure "heightmap.png" is in the correct directory
        exit( EXIT_FAILURE );
    }
    if ( ( _trackFilter = _loadAndRegisterTrackFilter( "trackFilter.png" ) ) == -1 )
    {
        exit( EXIT_FAILURE );
    }
    if ( ( _trackTexture = _loadAndRegisterTexture( "roadTexture2.jpg" ) ) == -1 )
    {
        exit( EXIT_FAILURE );
    }
    if ( ( _easterEggTexture = _loadAndRegisterTexture( "rainbowRoadTexture.jpg" ) ) == -1 )
    {
        exit( EXIT_FAILURE );
    }
    if ( ( _sceneTexture = _loadAndRegisterTrackFilter( "grassTexture.jpg" ) ) == -1 )
    {
        exit( EXIT_FAILURE );
    }
    _skyTex = _loadAndRegisterSkyboxTexture( "cubeMapFrozen.jpg" );
    // Now create the ground buffers using the loaded height map data
    _createGroundBuffers( );

    // Generate other environment elements
    _generateEnvironment( );

    _createSkyboxBuffers( );

    _createSpeedLineBuffers( );
}

void FPEngine::mSetupScene( )
{
    _cameraSpeed = glm::vec2( 0.25f, 0.02f );

    // Calculate bezier curve points
    int xc = ( 112 + 558 ) / 2;
    int zc = ( 85 + 577 ) / 2;

    int xr = ( 558 - 112 ) / 5;
    int zr = ( 577 - 85 ) / 5;

    float scaleFactor = 1.2f;

    // Curve 1
    _p0 = _convertToWorldCoords( xc, zc + zr );
    _p1 = _convertToWorldCoords( (int)( xc - k * xr ), zc + zr );
    _p1 = _p0 + ( _p1 - _p0 ) * ( scaleFactor + 0.2f );
    _p2 = _convertToWorldCoords( xc - xr, (int)( zc + k * zr ) );
    _p2 = _p0 + ( _p2 - _p0 ) * ( scaleFactor );
    _p3 = _convertToWorldCoords( xc - xr, zc );
    _p3 = _p0 + ( _p3 - _p0 ) * ( scaleFactor );

    // Curve 2
    _p4 = _convertToWorldCoords( xc - xr, (int)( zc - k * zr ) );
    _p4 = _p0 + ( _p4 - _p0 ) * scaleFactor;
    _p5 = _convertToWorldCoords( (int)( xc - k * xr ), zc - zr );
    _p5 = _p0 + ( _p5 - _p0 ) * scaleFactor;
    _p6 = _convertToWorldCoords( xc, zc - zr );
    _p6 = _p0 + ( _p6 - _p0 ) * scaleFactor;

    // Curve 3
    _p7 = _convertToWorldCoords( (int)( xc + k * xr ), zc - zr );
    _p7 = _p0 + ( _p7 - _p0 ) * scaleFactor;
    _p8 = _convertToWorldCoords( xc + xr, (int)( zc - k * zr ) );
    _p8 = _p0 + ( _p8 - _p0 ) * scaleFactor;
    _p9 = _convertToWorldCoords( xc + xr, zc );
    _p9 = _p0 + ( _p9 - _p0 ) * ( scaleFactor + 0.4f );

    // Curve 4
    _p10 = _convertToWorldCoords( xc + xr, (int)( zc + k * zr ) );
    _p10 = _p0 + ( _p10 - _p0 ) * ( scaleFactor + 0.6f );
    _p11 = _convertToWorldCoords( (int)( xc + k * xr ), zc + zr );
    _p11 = _p0 + ( _p11 - _p0 ) * ( scaleFactor + 0.6f );
    _p12 = _p0;

    _circleControlPoints = { _p0, _p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8, _p9, _p10, _p11, _p12 };

    // TODO: Remove this test code before turning in!
    std::vector<glm::vec3> curvePoints;
    int resolution = 100; // number of samples per curve
    for ( int curveIndex = 0; curveIndex < numCurves; ++curveIndex )
    {
        // get the appropriate control points for this curve segment:
        glm::vec3 c0 = _circleControlPoints[curveIndex * 3 + 0];
        glm::vec3 c1 = _circleControlPoints[curveIndex * 3 + 1];
        glm::vec3 c2 = _circleControlPoints[curveIndex * 3 + 2];
        glm::vec3 c3 = _circleControlPoints[curveIndex * 3 + 3];

        for ( int i = 0; i <= resolution; ++i )
        {
            float t      = (float)i / (float)resolution;
            glm::vec3 pt = evaluateBezier( t, c0, c1, c2, c3 );
            pt.y += 10;
            curvePoints.push_back( pt );
        }
    }

    // Now create a VAO/VBO for these points
    glGenVertexArrays( 1, &_curveVAO );
    glBindVertexArray( _curveVAO );

    glGenBuffers( 1, &_curveVBO );
    glBindBuffer( GL_ARRAY_BUFFER, _curveVBO );
    glBufferData( GL_ARRAY_BUFFER, curvePoints.size( ) * sizeof( glm::vec3 ), curvePoints.data( ), GL_STATIC_DRAW );

    glEnableVertexAttribArray( _lightingShaderAttributeLocations.vPos );
    glVertexAttribPointer( _lightingShaderAttributeLocations.vPos, 3, GL_FLOAT, GL_FALSE, sizeof( glm::vec3 ), (void*)0 );

    // No normals needed since we will just draw a line
    glBindVertexArray( 0 );

    // store number of curve points in a variable for rendering
    _curvePointCount = (GLsizei)curvePoints.size( );

    // --------------------------------------------------------------

    // Initialize starting positions of the carts
    _startPosition = evaluateBezier( 0.0f, _p0, _p1, _p2, _p3 );

    _aiCartPosition = _startPosition;
    _pPlayerCar->setPosition( _startPosition );

    // Set forward direction of carts
    glm::vec3 nextPos   = evaluateBezier( _startT + 0.001f, _p0, _p1, _p2, _p3 );
    glm::vec3 direction = glm::normalize( nextPos - _startPosition );
    fprintf( stdout, "Player Car Direction, (X: %f)\n", direction.x );
    _pPlayerCar->setForwardDirection( glm::vec3( 1.0f, 0.0f, 0.0f ) );

    _cams[CAM_ID::ARC_CAM] = new CSCI441::ArcballCam( );
    _cams[CAM_ID::ARC_CAM]->setLookAtPoint( _pPlayerCar->getPosition( ) );
    _cams[CAM_ID::ARC_CAM]->setRadius( 15 );
    _cams[CAM_ID::ARC_CAM]->setTheta( -M_PI / 3.0f );
    _cams[CAM_ID::ARC_CAM]->setPhi( 8 * M_PI / 6 );
    _cams[CAM_ID::ARC_CAM]->recomputeOrientation( );

    _cams[CAM_ID::FIXED_CAM] = new CSCI441::ArcballCam( );
    _cams[CAM_ID::FIXED_CAM]->setLookAtPoint( _pPlayerCar->getPosition( ) );
    _cams[CAM_ID::FIXED_CAM]->setRadius( 15 );
    _cams[CAM_ID::FIXED_CAM]->setTheta( M_PI / 2 );
    _cams[CAM_ID::FIXED_CAM]->setPhi( 8 * M_PI / 6 );
    _cams[CAM_ID::FIXED_CAM]->recomputeOrientation( );

    _cams[CAM_ID::FPV_CAM] = new CSCI441::FreeCam();
    _cams[CAM_ID::FPV_CAM]->setPosition(_pPlayerCar->getPosition() + glm::vec3(2.0f,0.0f,0.0f));
    _cams[CAM_ID::FPV_CAM]->setTheta(-M_PI / 2 );
    _cams[CAM_ID::FPV_CAM]->setPhi( 3*M_PI / 2 );
    _cams[CAM_ID::FPV_CAM]->recomputeOrientation( );

    camID = CAM_ID::FIXED_CAM;

    // TODO #6: set lighting uniforms
    glm::vec3 lightDirection = glm::vec3( -1.0f, -1.0f, -1.0f );
    glm::vec3 lightColor     = glm::vec3( 1.0f, 1.0f, 1.0f );
    glProgramUniform3fv( _lightingShaderProgram->getShaderProgramHandle( ), _lightingShaderUniformLocations.lightColor, 1, glm::value_ptr( lightColor ) );
    glProgramUniform3fv( _lightingShaderProgram->getShaderProgramHandle( ), _lightingShaderUniformLocations.lightPosition, 1, glm::value_ptr( lightDirection ) );

    //******************************************************************
    glm::vec3 playerPosition     = _pPlayerCar->getPosition( );
    glm::vec3 spotLightPosition  = glm::vec3( -110.0f, 30.0f, -110.0f ); // Spotlight above the Being
    glm::vec3 spotLightDirection = glm::vec3( 1.0f, 0.0f, 0.0f );
    GLfloat spotLightCutoff      = glm::cos( glm::radians( 90.0f ) );
    GLfloat spotLightOuterCutoff = glm::cos( glm::radians( 120.0f ) );

    glProgramUniform3fv( _lightingShaderProgram->getShaderProgramHandle( ), _lightingShaderUniformLocations.spotLightPosition, 1, glm::value_ptr( spotLightPosition ) );

    glProgramUniform3fv( _lightingShaderProgram->getShaderProgramHandle( ), _lightingShaderUniformLocations.spotLightDirection, 1, glm::value_ptr( spotLightDirection ) );

    glProgramUniform1f( _lightingShaderProgram->getShaderProgramHandle( ), _lightingShaderUniformLocations.spotLightOuterCutoff, spotLightOuterCutoff );

    glProgramUniform1f( _lightingShaderProgram->getShaderProgramHandle( ), _lightingShaderUniformLocations.spotLightCutoff, spotLightCutoff );

    // Pass lighting data to terrain shader
    glProgramUniform3fv( _terrainShaderProgram->getShaderProgramHandle( ), _terrainShaderUniformLocations.lightPosition, 1, glm::value_ptr( lightDirection ) );
    glProgramUniform3fv( _terrainShaderProgram->getShaderProgramHandle( ), _terrainShaderUniformLocations.lightColor, 1, glm::value_ptr( lightColor ) );
    glProgramUniform3fv( _terrainShaderProgram->getShaderProgramHandle( ), _terrainShaderUniformLocations.camPosition, 1, glm::value_ptr( _cams[camID]->getPosition( ) ) );
    // Pass spotlight data
    glProgramUniform3fv( _terrainShaderProgram->getShaderProgramHandle( ), _terrainShaderUniformLocations.spotLightPosition, 1, glm::value_ptr( spotLightPosition ) );
    glProgramUniform3fv( _terrainShaderProgram->getShaderProgramHandle( ), _terrainShaderUniformLocations.spotLightDirection, 1, glm::value_ptr( spotLightDirection ) );
    glProgramUniform1f( _terrainShaderProgram->getShaderProgramHandle( ), _terrainShaderUniformLocations.spotLightOuterCutoff, spotLightOuterCutoff );
    glProgramUniform1f( _terrainShaderProgram->getShaderProgramHandle( ), _terrainShaderUniformLocations.spotLightCutoff, spotLightCutoff );

    // Precompute Arc Length Parameterization lookup table
    int NUM_SAMPLES = 1000;
    _arcLengths     = std::vector<float>( NUM_SAMPLES + 1, 0.0f );
    _sampleTs       = std::vector<float>( NUM_SAMPLES + 1, 0.0f );

    for ( int i = 0; i <= NUM_SAMPLES; ++i )
    {
        float t      = (float)i / (float)NUM_SAMPLES;
        _sampleTs[i] = t;
    }

    glm::vec3 prevPoint = evaluateBezier( 0.0f, _p0, _p1, _p2, _p3 );
    _totalLength        = 0.0f;
    _arcLengths[0]      = 0.0f;

    for ( int i = 1; i <= NUM_SAMPLES; ++i )
    {
        glm::vec3 currPoint = evaluateBezier( _sampleTs[i], _p0, _p1, _p2, _p3 );
        float segmentLength = glm::length( currPoint - prevPoint );
        _totalLength += segmentLength;
        _arcLengths[i] = _totalLength;
        prevPoint      = currPoint;
    }

    printf( "Directional Light Color: (%f, %f, %f)\n", lightColor.x, lightColor.y, lightColor.z );
    printf( "Directional Light Direction: (%f, %f, %f)\n", lightDirection.x, lightDirection.y, lightDirection.z );
    printf( "Spotlight Position: (%f, %f, %f)\n", spotLightPosition.x, spotLightPosition.y, spotLightPosition.z );
    printf( "Spotlight Direction: (%f, %f, %f)\n", spotLightDirection.x, spotLightDirection.y, spotLightDirection.z );
    printf( "Spotlight Cutoffs: Inner %f, Outer %f\n", spotLightCutoff, spotLightOuterCutoff );
}

void FPEngine::_generateTrees( const char* FILENAME, GLint GRID_WIDTH, GLint GRID_HEIGHT, GLfloat GRID_SPACING_WIDTH, GLfloat GRID_SPACING_HEIGHT )
{

    // enable setting to prevent image from being upside down
    stbi_set_flip_vertically_on_load( true );

    // will hold image parameters after load
    GLint imageWidth, imageHeight, imageChannels;
    // load image from file
    GLubyte* data = stbi_load( FILENAME, &imageWidth, &imageHeight, &imageChannels, 1 );
    // if data was read from file

    fprintf( stdout, "Tree File Loaded, Image Width: %d Image Height: %d\n", imageWidth, imageHeight );

    if ( !data )
    {
        fprintf( stdout, "Error Loading Track Filter for Tree Generations\n" );
        return;
    }

    GLuint _treeFilter[imageWidth][imageHeight];

    for ( int i = 0; i < imageHeight * imageWidth; i++ )
    {
        _treeFilter[i % imageWidth][i / imageHeight] = static_cast<GLuint>( data[i] );
    }
    const GLfloat LEFT_END_POINT   = -GRID_WIDTH / 2.0f - 5.0f;
    const GLfloat RIGHT_END_POINT  = GRID_WIDTH / 2.0f + 5.0f;
    const GLfloat BOTTOM_END_POINT = -GRID_HEIGHT / 2.0f - 5.0f;
    const GLfloat TOP_END_POINT    = GRID_HEIGHT / 2.0f + 5.0f;

    GLfloat gridWidthToTrack  = imageWidth / ( GRID_WIDTH + 10.0f );
    GLfloat gridHeightToTrack = imageHeight / ( GRID_HEIGHT + 10.0f );

    for ( int i = LEFT_END_POINT; i <= RIGHT_END_POINT; i += GRID_SPACING_WIDTH )
    {
        for ( int j = BOTTOM_END_POINT; j <= TOP_END_POINT; j += GRID_SPACING_HEIGHT )
        {
            if ( i % 2 == 0 && j % 2 == 0 && gen_rand_GLfloat( ) < 0.01f )
            {
                if ( _treeFilter[(int)( ( i + RIGHT_END_POINT ) * gridWidthToTrack )][(int)( ( j + TOP_END_POINT ) * gridHeightToTrack )] != 0.0f )
                {
                    _trees.push_back( Tree( glm::vec3( i, getTerrainHeight( i, j ), j ),
                                            _lightingShaderProgram->getShaderProgramHandle( ),
                                            _lightingShaderUniformLocations.mvpMatrix,
                                            _lightingShaderAttributeLocations.vNorm,
                                            _lightingShaderUniformLocations.materialDiffuse,
                                            _lightingShaderUniformLocations.materialSpecular,
                                            _lightingShaderUniformLocations.materialShine,
                                            _lightingShaderUniformLocations.isEmitter ) );
                }
            }
        }
    }
    stbi_image_free( data );
    fprintf( stdout, "Tree generation End\n" );
}

bool FPEngine::loadHeightMap( const std::string& filepath )
{
    unsigned char* data = stbi_load( filepath.c_str( ), &heightMapWidth, &heightMapHeight, &heightMapChannels, 1 ); // Load as grayscale
    if ( !data )
    {
        fprintf( stderr, "Failed to load height map: %s\n", filepath.c_str( ) );
        return false;
    }

    // Create OpenGL texture
    glGenTextures( 1, &_heightMapTextureID );
    glBindTexture( GL_TEXTURE_2D, _heightMapTextureID );

    // Set texture parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // Upload texture data
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, heightMapWidth, heightMapHeight, 0, GL_RED, GL_UNSIGNED_BYTE, data );
    glGenerateMipmap( GL_TEXTURE_2D );

    // Populate heightData for CPU-side height queries
    heightData.resize( heightMapWidth * heightMapHeight );
    for ( int i = 0; i < heightMapWidth * heightMapHeight; ++i )
    {
        heightData[i] = static_cast<float>( data[i] ) / 255.0f * maxHeight;
    }

    stbi_image_free( data );
    return true;
}

GLuint FPEngine::_loadAndRegisterTexture( const char* FILENAME )
{
    // our handle to the GPU
    GLuint textureHandle = 0;

    // enable setting to prevent image from being upside down
    stbi_set_flip_vertically_on_load( true );

    // will hold image parameters after load
    GLint imageWidth, imageHeight, imageChannels;
    // load image from file
    GLubyte* data = stbi_load( FILENAME, &imageWidth, &imageHeight, &imageChannels, 0 );
    // if data was read from file
    if ( data )
    {
        const GLint STORAGE_TYPE = ( imageChannels == 4 ? GL_RGBA : GL_RGB );
        glGenTextures( 1, &textureHandle );
        glBindTexture( GL_TEXTURE_2D, textureHandle );
        // set texture parameters
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

        glTexImage2D( GL_TEXTURE_2D, 0, STORAGE_TYPE, imageWidth, imageHeight, 0, STORAGE_TYPE, GL_UNSIGNED_BYTE, data );

        fprintf( stdout, "[INFO]: %s texture map read in with handle %d\n", FILENAME, textureHandle );

        // release image memory from CPU - it now lives on the GPU
        stbi_image_free( data );
    }
    else
    {
        // load failed
        fprintf( stderr, "[ERROR]: Could not load texture map \"%s\"\n", FILENAME );
    }

    // return generated texture handle
    return textureHandle;
}

GLuint FPEngine::_loadAndRegisterTrackFilter( const char* FILENAME )
{
    // our handle to the GPU
    GLuint textureHandle = 0;

    // enable setting to prevent image from being upside down
    stbi_set_flip_vertically_on_load( true );

    // will hold image parameters after load
    GLint imageWidth, imageHeight, imageChannels;
    // load image from file
    GLubyte* data = stbi_load( FILENAME, &imageWidth, &imageHeight, &imageChannels, 0 );
    // if data was read from file
    if ( data )
    {
        const GLint STORAGE_TYPE = ( imageChannels == 1 ? GL_DEPTH_COMPONENT : GL_RGB );
        glGenTextures( 1, &textureHandle );
        glBindTexture( GL_TEXTURE_2D, textureHandle );
        // set texture parameters
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

        glTexImage2D( GL_TEXTURE_2D, 0, STORAGE_TYPE, imageWidth, imageHeight, 0, STORAGE_TYPE, GL_UNSIGNED_BYTE, data );

        fprintf( stdout, "[INFO]: %s height map read in with handle %d\n", FILENAME, textureHandle );

        // release image memory from CPU - it now lives on the GPU
        stbi_image_free( data );
    }
    else
    {
        // load failed
        fprintf( stderr, "[ERROR]: Could not load texture map \"%s\"\n", FILENAME );
    }

    // return generated texture handle
    return textureHandle;
}

float FPEngine::getTerrainHeight( float x, float z ) const
{
    // Map world coordinates to heightmap coordinates
    float terrainX = x + _centerX;
    float terrainZ = z + _centerZ;

    // Clamp the coordinates to the height map boundaries
    if ( terrainX < 0.0f )
        terrainX = 0.0f;
    if ( terrainZ < 0.0f )
        terrainZ = 0.0f;
    if ( terrainX >= heightMapWidth - 1 )
        terrainX = heightMapWidth - 1.001f; // Prevent overflow
    if ( terrainZ >= heightMapHeight - 1 )
        terrainZ = heightMapHeight - 1.001f;

    // Get the integer and fractional parts
    int x0   = static_cast<int>( floor( terrainX ) );
    int z0   = static_cast<int>( floor( terrainZ ) );
    float tx = terrainX - x0;
    float tz = terrainZ - z0;

    // Retrieve heights from the four surrounding vertices
    float h00 = heightData[z0 * heightMapWidth + x0];
    float h10 = heightData[z0 * heightMapWidth + ( x0 + 1 )];
    float h01 = heightData[( z0 + 1 ) * heightMapWidth + x0];
    float h11 = heightData[( z0 + 1 ) * heightMapWidth + ( x0 + 1 )];

    // Bi-linear interpolation
    float h0                 = h00 * ( 1 - tx ) + h10 * tx;
    float h1                 = h01 * ( 1 - tx ) + h11 * tx;
    float interpolatedHeight = h0 * ( 1 - tz ) + h1 * tz;

    return interpolatedHeight;
}

void FPEngine::_generateEnvironment( )
{
    //******************************************************************
    // parameters to make up our grid size and spacing, feel free to
    // play around with this
    const GLfloat GRID_WIDTH          = WORLD_SIZE * 1.8f;
    const GLfloat GRID_LENGTH         = WORLD_SIZE * 1.8f;
    const GLfloat GRID_SPACING_WIDTH  = 1.0f;
    const GLfloat GRID_SPACING_LENGTH = 1.0f;
    // precomputed parameters based on above
    const GLfloat LEFT_END_POINT   = -GRID_WIDTH / 2.0f - 5.0f;
    const GLfloat RIGHT_END_POINT  = GRID_WIDTH / 2.0f + 5.0f;
    const GLfloat BOTTOM_END_POINT = -GRID_LENGTH / 2.0f - 5.0f;
    const GLfloat TOP_END_POINT    = GRID_LENGTH / 2.0f + 5.0f;
    //******************************************************************

    //******************************************************************
    // draws a grid as our ground plane
    // do not edit this next section

    _generateTrees( "trackFilter.png", heightMapHeight, heightMapWidth, GRID_SPACING_WIDTH, GRID_SPACING_LENGTH );

    _gridColor = glm::vec3( 1.0f, 1.0f, 1.0f );
    //******************************************************************
}

//*************************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!
void FPEngine::_computeAndSendMatrixUniforms( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const
{
    // For lighting shader
    _lightingShaderProgram->setProgramUniform( _lightingShaderUniformLocations.modelMatrix, modelMtx );
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    _lightingShaderProgram->setProgramUniform( _lightingShaderUniformLocations.mvpMatrix, mvpMtx );
    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx ) ) );
    _lightingShaderProgram->setProgramUniform( _lightingShaderUniformLocations.vNormalMatrix, normalMtx );
    // Set camera position uniform
    _lightingShaderProgram->setProgramUniform( _lightingShaderUniformLocations.camPosition, _cams[camID]->getPosition( ) );

    // For terrain shader
    _terrainShaderProgram->setProgramUniform( _terrainShaderUniformLocations.mvpMatrix, mvpMtx );
    _terrainShaderProgram->setProgramUniform( _terrainShaderUniformLocations.modelMatrix, modelMtx );
    _terrainShaderProgram->setProgramUniform( _terrainShaderUniformLocations.normalMatrix, normalMtx );
    _terrainShaderProgram->setProgramUniform( _terrainShaderUniformLocations.camPosition, _cams[camID]->getPosition( ) );
}

void FPEngine::_renderScene( glm::mat4 viewMtx, glm::mat4 projMtx ) const
{
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
    _terrainShaderProgram->useProgram( );

    // Bind heightmap texture
    glActiveTexture( GL_TEXTURE0 + HEIGHT_MAP_SLOT );
    glBindTexture( GL_TEXTURE_2D, _heightMapTextureID );
    glActiveTexture( GL_TEXTURE0 + TRACK_FILTER_SLOT );
    glBindTexture( GL_TEXTURE_2D, _trackFilter );
    glActiveTexture( GL_TEXTURE0 + TRACK_TEXTURE_SLOT );
    glBindTexture( GL_TEXTURE_2D, _trackTexture );
    glActiveTexture( GL_TEXTURE0 + EASTER_TEXTURE_SLOT );
    glBindTexture( GL_TEXTURE_2D, _easterEggTexture );
    glActiveTexture( GL_TEXTURE0 + SCENE_TEXTURE_SLOT );
    glBindTexture( GL_TEXTURE_2D, _sceneTexture );
    _terrainShaderProgram->setProgramUniform( _terrainShaderUniformLocations.heightMap, HEIGHT_MAP_SLOT );
    _terrainShaderProgram->setProgramUniform( _terrainShaderUniformLocations.trackFilter, TRACK_FILTER_SLOT );
    _terrainShaderProgram->setProgramUniform( _terrainShaderUniformLocations.trackTexture, TRACK_TEXTURE_SLOT );
    _terrainShaderProgram->setProgramUniform( _terrainShaderUniformLocations.sceneTexture, SCENE_TEXTURE_SLOT );
    _terrainShaderProgram->setProgramUniform( _terrainShaderUniformLocations.eggTexture, EASTER_TEXTURE_SLOT );


    glProgramUniform1i( _terrainShaderProgram->getShaderProgramHandle( ), _terrainShaderUniformLocations.isReverse, isReverse );
    glm::vec2 texelSize( 1.0f / static_cast<float>( heightMapWidth ), 1.0f / static_cast<float>( heightMapHeight ) );
    _terrainShaderProgram->setProgramUniform( _terrainShaderUniformLocations.texelSize, texelSize );

    // Set uniforms
    _terrainShaderProgram->setProgramUniform( _terrainShaderUniformLocations.maxHeight, maxHeight );

    // Set material color
    glm::vec3 terrainColor( 0.5f, 0.8f, 0.2f ); // The color of the terrain
    _terrainShaderProgram->setProgramUniform( _terrainShaderUniformLocations.materialColor, terrainColor );

    // Set matrices
    glm::mat4 modelMtxTerrain = glm::mat4( 1.0f );
    _computeAndSendMatrixUniforms( modelMtxTerrain, viewMtx, projMtx );

    // Draw the terrain
    glBindVertexArray( _terrainVAO );
    glDrawElements( GL_TRIANGLES, terrainIndices.size( ), GL_UNSIGNED_INT, 0 );
    glBindVertexArray( 0 );

    // 2. Draw Car with Lighting Shader
    _lightingShaderProgram->useProgram( );

    for ( Tree curTree : _trees )
    {
        curTree.draw( viewMtx, projMtx );
    }

    // Compute and send matrix uniforms for lighting shader
    glm::mat4 modelMtxCar = glm::mat4( 1.0f );

    modelMtxCar = glm::translate( modelMtxCar, _pPlayerCar->getPosition( ) );
    _computeAndSendMatrixUniforms( modelMtxCar, viewMtx, projMtx );

    // Draw the car
    _pPlayerCar->draw( modelMtxCar, viewMtx, projMtx );

    // Compute transformations for the AI cart
    glm::mat4 modelMtxAICar = glm::mat4( 1.0f );

    glm::vec3 _aiCartDirection2 = glm::vec3( _aiCartDirection.x, 0, _aiCartDirection.z );

    _pAICar->setForwardDirection( glm::normalize( _aiCartDirection2 ) );

    // Combine orientation with the model matrix for drawing
    modelMtxAICar = glm::translate( glm::mat4( 1.0f ), _aiCartPosition ) * glm::scale( glm::mat4( 1.0f ), glm::vec3( 1.0f ) );

    _computeAndSendMatrixUniforms( modelMtxAICar, viewMtx, projMtx );

    _pAICar->draw( modelMtxAICar, viewMtx, projMtx );

    float currentPlayerSpeed = _playerSpeed;

    if ( currentPlayerSpeed > _speedThreshold )
    {
        _speedLineShaderProgram->useProgram();

        // Update speed line vertices
        std::vector<glm::vec3> speedLineVertices;
        int numPoints = 100;
        float spacing = 0.1f;
        for (int i = 0; i < numPoints; ++i)
        {
            float t = (float)i / (float)(numPoints - 1);
            // Position the speed lines slightly behind the car based on direction
            glm::vec3 offset = -normalize(_pPlayerCar->getForwardDirection()) * spacing * (float)i;
            glm::vec3 point = _pPlayerCar->getPosition() + offset;
            // Adjust y-coordinate to match terrain height
            point.y = getTerrainHeight(point.x, point.z) + 0.5f;
            speedLineVertices.push_back(point);
        }

        glBindBuffer(GL_ARRAY_BUFFER, _speedLineVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, speedLineVertices.size() * sizeof(glm::vec3), speedLineVertices.data());

        // Set shader uniforms
        glm::mat4 modelMtxSpeedLines = glm::mat4(1.0f);
        glm::mat4 mvpMatrix = projMtx * viewMtx * modelMtxSpeedLines;
        _speedLineShaderProgram->setProgramUniform("mvpMatrix", mvpMatrix);

        // Enable blending for transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Draw speed lines as lines, which the geometry shader will convert to quads
        glBindVertexArray(_speedLineVAO);
        glDrawArrays(GL_LINES, 0, speedLineVertices.size());
        glBindVertexArray(0);

        glDisable(GL_BLEND);
    }
}

void FPEngine::_updateScene( )
{

    double currentTime = glfwGetTime( );
    double deltaTime   = currentTime - _lastTime;
    _lastTime          = currentTime;

    glm::vec3 playerPos = _pPlayerCar->getPosition( );

    // Check Checkpoints
    if ( !startCheckpoint )
    {
        if ( abs( playerPos.x ) < 5.0f && playerPos.z > 0 )
        {
            startCheckpoint      = true;
            checkOrder[numOrder] = 0;
            numOrder++;
            fprintf( stdout, "start\n" );
        }
    }
    if ( !firstCheckpoint )
    {
        if ( abs( playerPos.z ) < 5.0f && playerPos.x > 0 )
        {
            firstCheckpoint      = true;
            checkOrder[numOrder] = 1;
            numOrder++;
            fprintf( stdout, "1st\n" );
        }
    }
    if ( !secondCheckpoint )
    {
        if ( abs( playerPos.x ) < 5.0f && playerPos.z < 0 )
        {
            secondCheckpoint     = true;
            checkOrder[numOrder] = 2;
            numOrder++;
            fprintf( stdout, "2nd\n" );
        }
    }
    if ( !thirdCheckpoint )
    {
        if ( abs( playerPos.z ) < 5.0f && playerPos.x < 0 )
        {
            thirdCheckpoint      = true;
            checkOrder[numOrder] = 3;
            numOrder++;
            fprintf( stdout, "3rd\n" );
        }
    }

    if ( numOrder == 4 && ( abs( playerPos.x ) < 5.0f && playerPos.z > 0 ))
    {
        fprintf( stdout, "Checking\n" );
        numOrder         = 1;
        isReverse        = 1;
        firstCheckpoint  = false;
        secondCheckpoint = false;
        thirdCheckpoint  = false;
        if ( checkOrder[1] == 1 )
            isReverse = 0;
        fprintf( stdout, "isReverse: %d\n", isReverse );
    }

    // Retrieve the terrain height at the plane's current X and Z
    float terrainHeightPlayer = getTerrainHeight( playerPos.x, playerPos.z );

    // Define an offset to keep the plane above the terrain
    float heightOffset = 0.5f;

    //  Ensure the player stays above the terrain
    if ( playerPos.y < terrainHeightPlayer + heightOffset || playerPos.y > terrainHeightPlayer + heightOffset )
    {
        playerPos.y = terrainHeightPlayer + heightOffset;
        _pPlayerCar->setPosition( playerPos );
    }
    // keeping the original functionality with moving the cam
    if ( _keys[GLFW_KEY_1] )
    {
        // Switch to Fixed
        camID = CAM_ID::FIXED_CAM;
    }
    else if ( _keys[GLFW_KEY_2] )
    {
        // Switch to arc
        camID = CAM_ID::ARC_CAM;
    }
    else if (_keys[GLFW_KEY_3])
    {
        camID = CAM_ID::FPV_CAM;
    }
    if ( _keys[GLFW_KEY_W] )
    {
        // Increase player speed until max speed
        _playerSpeed += _playerAcceleration * (float)deltaTime;
        if ( _playerSpeed > _playerMaxSpeed )
            _playerSpeed = _playerMaxSpeed;

        _pPlayerCar->moveForward( _playerSpeed );
        _pPlayerCar->setForwardDirection( );

        if ( !( _pPlayerCar->getPosition( ).x + 0.2f < 100.0f && _pPlayerCar->getPosition( ).z + 0.2f < 100.0f && _pPlayerCar->getPosition( ).x + 0.2f > -100.0f &&
                _pPlayerCar->getPosition( ).z + 0.2f > -100.0f ) )
        { // bounds checking, so that we can stay within the created world
            _pPlayerCar->_isFalling = true;
        }
    }
    else if ( _keys[GLFW_KEY_S] )
    {
        _playerSpeed -= _playerAcceleration * (float)deltaTime;
        if ( _playerSpeed > _playerMaxSpeed )
            _playerSpeed = _playerMaxSpeed;

        _pPlayerCar->moveForward( _playerSpeed );
        _pPlayerCar->setForwardDirection( );

        if ( !( _pPlayerCar->getPosition( ).x + 0.2f < 100.0f && _pPlayerCar->getPosition( ).z + 0.2f < 100.0f && _pPlayerCar->getPosition( ).x + 0.2f > -100.0f &&
                _pPlayerCar->getPosition( ).z + 0.2f > -100.0f ) )
        { // bounds checking, so that we can stay within the created world
            _pPlayerCar->_isFalling = true;
        }
    }
    else
    {
        // If not pressing W, maybe slow down gradually
        if ( _playerSpeed < 0.0f )
            _playerSpeed += _playerAcceleration * (float)deltaTime;
            _pPlayerCar->moveForward( _playerSpeed );
        if ( _playerSpeed > 0.0f )
        {
            _playerSpeed -= _playerAcceleration * (float)deltaTime;
            _pPlayerCar->moveForward( _playerSpeed );
            _pPlayerCar->setForwardDirection( );
        }
        else
        {
            _pPlayerCar->notMoving( );
        }
    }
    if ( _keys[GLFW_KEY_D] )
    {
        if ( _pPlayerCar->isMoving )
        {
            _pPlayerCar->rotateSelf( -0.1f ); // give the axis of travel and whether the axis involves the A key as then we need to inverse the angle
            _pPlayerCar->setForwardDirection( );
            _cams[CAM_ID::FIXED_CAM]->setTheta( _cams[CAM_ID::FIXED_CAM]->getTheta( ) + 0.1f );
            _cams[CAM_ID::FPV_CAM]->setTheta( _cams[CAM_ID::FPV_CAM]->getTheta( ) + 0.1f );

        }
        _pPlayerCar->isTurnRight = true;
    }
    else
    {
        _pPlayerCar->isTurnRight = false;
    }
    if ( _keys[GLFW_KEY_A] )
    {
        if ( _pPlayerCar->isMoving )
        {
            _pPlayerCar->rotateSelf( 0.1f ); // give the axis of travel and whether the axis involves the A key as then we need to inverse the angle
            _pPlayerCar->setForwardDirection( );
            _cams[CAM_ID::FIXED_CAM]->setTheta( _cams[CAM_ID::FIXED_CAM]->getTheta( ) - 0.1f );
            _cams[CAM_ID::FPV_CAM]->setTheta( _cams[CAM_ID::FPV_CAM]->getTheta( ) - 0.1f );

        }
        _pPlayerCar->isTurnLeft = true;
    }
    else
    {
        _pPlayerCar->isTurnLeft = false;
    }
    _pPlayerCar->setForwardDirection( );
    if(camID != CAM_ID::FPV_CAM) _cams[camID]->setLookAtPoint( _pPlayerCar->getPosition( ) );
    else _cams[camID]->setPosition(_pPlayerCar->getPosition() + 2.0f * _pPlayerCar->getForwardDirection());
    _cams[camID]->recomputeOrientation( );
    _pPlayerCar->update( );
    _moveSpotlight( );

    // Update distance by a delta time
    _aiSpeed += _aiAcceleration * (float)deltaTime;
    if ( _aiSpeed > _aiMaxSpeed )
        _aiSpeed = _aiMaxSpeed;
    _distanceTraveled -= _aiSpeed * (float)deltaTime;

    if ( _distanceTraveled < 0.0f )
    {
        _distanceTraveled = fmod( _distanceTraveled + _totalLength, _totalLength ); // loop around
    }

    _bezierT = arcLengthToT( _distanceTraveled, _arcLengths, _sampleTs );

    int currentCurve = (int)floor( _bezierT * (float)numCurves );
    float localT     = ( _bezierT * (float)numCurves ) - (float)currentCurve;

    glm::vec3 p0 = _circleControlPoints[currentCurve * 3];
    glm::vec3 p1 = _circleControlPoints[currentCurve * 3 + 1];
    glm::vec3 p2 = _circleControlPoints[currentCurve * 3 + 2];
    glm::vec3 p3 = _circleControlPoints[currentCurve * 3 + 3];

    _aiCartPosition = evaluateBezier( localT, p0, p1, p2, p3 );

    // So cart hugs the terrain
    float terrainHeightCart = getTerrainHeight( _aiCartPosition.x, _aiCartPosition.z );
    _aiCartPosition.y       = terrainHeightCart + 0.5f;

    // Now compute the direction (tangent) by evaluating slightly ahead along the curve:
    float smallStep    = 0.001f; // A small value for forward tangent checking
    float nextDistance = _distanceTraveled + smallStep;
    if ( nextDistance > _totalLength )
    {
        nextDistance = fmod( nextDistance, _totalLength );
    }

    // Convert nextDistance to nextT
    float nextT = arcLengthToT( nextDistance, _arcLengths, _sampleTs );

    // Find which curve segment for nextT
    int nextCurve    = (int)floor( nextT * (float)numCurves );
    float nextLocalT = ( nextT * (float)numCurves ) - (float)nextCurve;

    glm::vec3 q0 = _circleControlPoints[nextCurve * 3];
    glm::vec3 q1 = _circleControlPoints[nextCurve * 3 + 1];
    glm::vec3 q2 = _circleControlPoints[nextCurve * 3 + 2];
    glm::vec3 q3 = _circleControlPoints[nextCurve * 3 + 3];

    glm::vec3 nextPos = evaluateBezier( nextLocalT, q0, q1, q2, q3 );

    // Compute direction vector
    _aiCartDirection = glm::normalize( _aiCartPosition - nextPos );
}

void FPEngine::handleKeyEvent( GLint key, GLint action )
{
    if ( key != GLFW_KEY_UNKNOWN )
        _keys[key] = ( ( action == GLFW_PRESS ) || ( action == GLFW_REPEAT ) );

    if ( action == GLFW_PRESS )
    {
        switch ( key )
        {
            // quit!
            case GLFW_KEY_Q:
            case GLFW_KEY_ESCAPE: setWindowShouldClose( ); break;

            default: break; // suppress CLion warning
        }
    }
}

void FPEngine::handleMouseButtonEvent( GLint button, GLint action )
{
    // if the event is for the left mouse button
    if ( button == GLFW_MOUSE_BUTTON_LEFT )
    {
        // update the left mouse button's state
        _leftMouseButtonState = action;
    }
}

void FPEngine::handleCursorPositionEvent( glm::vec2 currMousePosition )
{
    // if mouse hasn't moved in the window, prevent camera from flipping out
    if ( _mousePosition.x == MOUSE_UNINITIALIZED )
    {
        _mousePosition = currMousePosition;
    }

    // if the left mouse button is being held down while the mouse is moving
    if ( _leftMouseButtonState == GLFW_PRESS )
    {
        if ( _keys[GLFW_KEY_LEFT_SHIFT] || _keys[GLFW_KEY_RIGHT_SHIFT] )
        { // this is to check if shift is being pressed so we can zoom
            if ( _mousePosition.y - currMousePosition.y < 0 )
            { // zoom in or out
                _cams[camID]->moveBackward( _cameraSpeed.x );
            }
            else
            {
                _cams[camID]->moveForward( _cameraSpeed.x );
            }
        }
        else
        {
            // rotate the camera by the distance the mouse moved
            if ( camID != CAM_ID::FIXED_CAM )
                _cams[camID]->rotate( ( currMousePosition.x - _mousePosition.x ) * 0.005f, ( _mousePosition.y - currMousePosition.y ) * 0.005f );
        }
    }

    // update the mouse position
    _mousePosition = currMousePosition;
}

GLuint FPEngine::_loadAndRegisterSkyboxTexture( const char* FILENAME )
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

void FPEngine::_createGroundBuffers( )
{
    // Define scaling factors
    float scaleX = 1.0f;
    float scaleZ = 1.0f;

    _centerX = ( heightMapWidth - 1 ) * scaleX / 2.0f;
    _centerZ = ( heightMapHeight - 1 ) * scaleZ / 2.0f;

    // Generate vertices
    for ( int z = 0; z < heightMapHeight; ++z )
    {
        for ( int x = 0; x < heightMapWidth; ++x )
        {
            float y    = 0.0f; // Height will be computed in the vertex shader
            float posX = x * scaleX - _centerX;
            float posZ = z * scaleZ - _centerZ;
            terrainVertices.push_back( posX ); // X
            terrainVertices.push_back( y );    // Y
            terrainVertices.push_back( posZ ); // Z

            // Placeholder normal
            terrainVertices.push_back( 0.0f ); // Normal X
            terrainVertices.push_back( 1.0f ); // Normal Y
            terrainVertices.push_back( 0.0f ); // Normal Z

            // Texture coordinates
            float u = (float)x / ( heightMapWidth - 1 );
            float v = (float)z / ( heightMapHeight - 1 );
            terrainVertices.push_back( u );
            terrainVertices.push_back( v );
        }
    }

    // Generate indices for triangles, excluding quads within the hole radius
    for ( int z = 0; z < heightMapHeight - 1; ++z )
    {
        for ( int x = 0; x < heightMapWidth - 1; ++x )
        {
            // Calculate positions for current quad
            float posX0 = x * scaleX - _centerX;
            float posZ0 = z * scaleZ - _centerZ;
            float posX1 = ( x + 1 ) * scaleX - _centerX;
            float posZ1 = ( z + 1 ) * scaleZ - _centerZ;

            // Compute distance from center for the quad's center
            float centerQuadX    = ( posX0 + posX1 ) / 2.0f;
            float centerQuadZ    = ( posZ0 + posZ1 ) / 2.0f;
            float distQuadCenter = sqrt( centerQuadX * centerQuadX + centerQuadZ * centerQuadZ );

            // Generate indices for the quad
            int topLeft     = z * heightMapWidth + x;
            int topRight    = topLeft + 1;
            int bottomLeft  = ( z + 1 ) * heightMapWidth + x;
            int bottomRight = bottomLeft + 1;

            // First triangle
            terrainIndices.push_back( topLeft );
            terrainIndices.push_back( bottomLeft );
            terrainIndices.push_back( topRight );

            // Second triangle
            terrainIndices.push_back( topRight );
            terrainIndices.push_back( bottomLeft );
            terrainIndices.push_back( bottomRight );
        }
    }

    // Calculate normals
    calculateTerrainNormals( );

    // Create and upload buffers
    glGenVertexArrays( 1, &_terrainVAO );
    glBindVertexArray( _terrainVAO );

    glGenBuffers( 1, &_terrainVBO );
    glBindBuffer( GL_ARRAY_BUFFER, _terrainVBO );
    glBufferData( GL_ARRAY_BUFFER, terrainVertices.size( ) * sizeof( float ), terrainVertices.data( ), GL_STATIC_DRAW );

    glGenBuffers( 1, &_terrainIBO );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _terrainIBO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, terrainIndices.size( ) * sizeof( unsigned int ), terrainIndices.data( ), GL_STATIC_DRAW );

    // Set attribute pointers while VAO is bound
    const GLsizei stride = 8 * sizeof( float ); // 3 pos + 3 normal + 2 texCoord

    // Position attribute
    glEnableVertexAttribArray( _terrainShaderAttributeLocations.vPos );
    glVertexAttribPointer( _terrainShaderAttributeLocations.vPos,
                           3,        // size (x, y, z)
                           GL_FLOAT, // type
                           GL_FALSE, // normalized?
                           stride,   // stride
                           (void*)0  // array buffer offset
    );

    // Normal attribute
    glEnableVertexAttribArray( _terrainShaderAttributeLocations.vNormal );
    glVertexAttribPointer( _terrainShaderAttributeLocations.vNormal,
                           3, // size (nx, ny, nz)
                           GL_FLOAT,
                           GL_FALSE,
                           stride,
                           (void*)( 3 * sizeof( float ) ) // Offset after positions (3 floats)
    );

    // Texture Coordinate attribute
    glEnableVertexAttribArray( _terrainShaderAttributeLocations.vTexCoord );
    glVertexAttribPointer( _terrainShaderAttributeLocations.vTexCoord,
                           2, // size (u, v)
                           GL_FLOAT,
                           GL_FALSE,
                           stride,
                           (void*)( 6 * sizeof( float ) ) // Offset after positions and normals (6 floats)
    );

    glBindVertexArray( 0 );
}

void FPEngine::calculateTerrainNormals( )
{
    // Initialize normals array
    std::vector<glm::vec3> normals( heightMapWidth * heightMapHeight, glm::vec3( 0.0f ) );

    // Iterate over each triangle in the terrain
    for ( int z = 0; z < heightMapHeight - 1; ++z )
    {
        for ( int x = 0; x < heightMapWidth - 1; ++x )
        {
            // Define vertices of the two triangles in the grid cell
            int topLeft     = z * heightMapWidth + x;
            int topRight    = z * heightMapWidth + ( x + 1 );
            int bottomLeft  = ( z + 1 ) * heightMapWidth + x;
            int bottomRight = ( z + 1 ) * heightMapWidth + ( x + 1 );

            // First Triangle (topLeft, bottomLeft, topRight)
            glm::vec3 v0 = glm::vec3( terrainVertices[topLeft * 8 + 0], terrainVertices[topLeft * 8 + 1], terrainVertices[topLeft * 8 + 2] );
            glm::vec3 v1 = glm::vec3( terrainVertices[bottomLeft * 8 + 0], terrainVertices[bottomLeft * 8 + 1], terrainVertices[bottomLeft * 8 + 2] );
            glm::vec3 v2 = glm::vec3( terrainVertices[topRight * 8 + 0], terrainVertices[topRight * 8 + 1], terrainVertices[topRight * 8 + 2] );

            glm::vec3 edge1      = v1 - v0;
            glm::vec3 edge2      = v2 - v0;
            glm::vec3 faceNormal = glm::normalize( glm::cross( edge1, edge2 ) );

            // Accumulate normals for each vertex
            normals[topLeft] += faceNormal;
            normals[bottomLeft] += faceNormal;
            normals[topRight] += faceNormal;

            // Second Triangle (bottomLeft, bottomRight, topRight)
            glm::vec3 v3 = glm::vec3( terrainVertices[bottomRight * 8 + 0], terrainVertices[bottomRight * 8 + 1], terrainVertices[bottomRight * 8 + 2] );

            glm::vec3 edge3       = v3 - v1;
            glm::vec3 edge4       = v2 - v1;
            glm::vec3 faceNormal2 = glm::normalize( glm::cross( edge3, edge4 ) );

            // Accumulate normals for each vertex
            normals[bottomLeft] += faceNormal2;
            normals[bottomRight] += faceNormal2;
            normals[topRight] += faceNormal2;
        }
    }

    // Normalize the accumulated normals
    for ( int i = 0; i < normals.size( ); ++i )
    {
        normals[i] = glm::normalize( normals[i] );
        // Corrected indexing: stride is 8 floats
        terrainVertices[i * 8 + 3] = normals[i].x;
        terrainVertices[i * 8 + 4] = normals[i].y;
        terrainVertices[i * 8 + 5] = normals[i].z;
    }

    // Update VBO with new normals
    glBindBuffer( GL_ARRAY_BUFFER, _terrainVBO );
    glBufferSubData( GL_ARRAY_BUFFER, 0, terrainVertices.size( ) * sizeof( float ), terrainVertices.data( ) );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void FPEngine::_moveSpotlight( )
{
    glm::vec3 spotLightPosition  = _pPlayerCar->getPosition( ) + ( _pPlayerCar->getForwardDirection( ) * 5.0f ); // Spotlight above the Being
    glm::vec3 spotLightDirection = _pPlayerCar->getForwardDirection( );

    glProgramUniform3fv( _lightingShaderProgram->getShaderProgramHandle( ), _lightingShaderUniformLocations.spotLightPosition, 1, glm::value_ptr( spotLightPosition ) );
    glProgramUniform3fv( _lightingShaderProgram->getShaderProgramHandle( ), _lightingShaderUniformLocations.spotLightDirection, 1, glm::value_ptr( spotLightDirection ) );

    glProgramUniform3fv( _terrainShaderProgram->getShaderProgramHandle( ), _terrainShaderUniformLocations.spotLightPosition, 1, glm::value_ptr( spotLightPosition ) );
    glProgramUniform3fv( _terrainShaderProgram->getShaderProgramHandle( ), _terrainShaderUniformLocations.spotLightDirection, 1, glm::value_ptr( spotLightDirection ) );
}

void FPEngine::run( )
{
    printf( "\nControls:\n" );
    printf( "\tW A S D - forwards, backwards, and side to side for the character in the world\n" );
    printf( "\tMouse Drag - Pan camera side to side and up and down\n" );
    printf( "\tMouse Drag up and down + shift - Zoom camera in and out\n" );
    printf( "\tQ / ESC - quit\n" );

    //  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
    //	until the user decides to close the window and quit the program.  Without a loop, the
    //	window will display once and then the program exits.
    while ( !glfwWindowShouldClose( mpWindow ) )
    {                                                         // check if the window was instructed to be closed
        glDrawBuffer( GL_BACK );                              // work with our back frame buffer
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // clear the current color contents and depth buffer in the window

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

        _renderScene( _cams[camID]->getViewMatrix( ), _cams[camID]->getProjectionMatrix( ) ); // draw everything to the window
        _updateScene( );
        glfwSwapBuffers( mpWindow ); // flush the OpenGL commands and make sure they get rendered!
        glfwPollEvents( );           // check for any events and signal to redraw screen
    }
}

void FPEngine::_createSkyboxBuffers( )
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

glm::vec3 FPEngine::evaluateBezier( float t, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3 )
{
    float u   = 1.0f - t;
    float tt  = t * t;
    float uu  = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    // Cubic Bezier formula
    glm::vec3 point = uuu * p0;
    point += 3.0f * uu * t * p1;
    point += 3.0f * u * tt * p2;
    point += ttt * p3;

    return point;
}

glm::vec3 FPEngine::_convertToWorldCoords( int px, int pz )
{
    float x = px - _trackCenter.x;
    float z = pz - _trackCenter.z;
    float y = getTerrainHeight( x, z );
    return { x, y, z };
}

float FPEngine::arcLengthToT( float s, const std::vector<float>& arcLengths, const std::vector<float>& sampleTs )
{
    // s should be in range [0, arcLengths.back()]
    // Binary search for the arc length interval
    int low  = 0;
    int high = (int)arcLengths.size( ) - 1;

    while ( low <= high )
    {
        int mid = ( low + high ) / 2;
        if ( arcLengths[mid] < s )
            low = mid + 1;
        else
            high = mid - 1;
    }

    // After this, low is the index of the first arc length >= s
    // Interpolate between low-1 and low
    if ( low == 0 )
        return 0.0f;
    if ( low == (int)arcLengths.size( ) )
        return 1.0f;

    float s0 = arcLengths[low - 1];
    float s1 = arcLengths[low];
    float t0 = sampleTs[low - 1];
    float t1 = sampleTs[low];

    float ratio = ( s - s0 ) / ( s1 - s0 );
    return t0 + ratio * ( t1 - t0 );
}

void FPEngine::_createSpeedLineBuffers() {
    glGenVertexArrays(1, &_speedLineVAO);
    glGenBuffers(1, &_speedLineVBO);

    glBindVertexArray(_speedLineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, _speedLineVBO);

    // Initialize with empty data; we'll update this each frame
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 100, NULL, GL_DYNAMIC_DRAW); // 100 points max

    // Position attribute
    glEnableVertexAttribArray(0); // Assuming location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

//*************************************************************************************
//
// Callbacks

void lab02_engine_keyboard_callback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
    auto engine = (FPEngine*)glfwGetWindowUserPointer( window );
    // pass the key and action through to the engine
    engine->handleKeyEvent( key, action );
}

void lab02_engine_cursor_callback( GLFWwindow* window, double x, double y )
{
    auto engine = (FPEngine*)glfwGetWindowUserPointer( window );

    // pass the cursor position through to the engine
    engine->handleCursorPositionEvent( glm::vec2( x, y ) );
}

void lab02_engine_mouse_button_callback( GLFWwindow* window, int button, int action, int mods )
{
    auto engine = (FPEngine*)glfwGetWindowUserPointer( window );

    // pass the mouse button and action through to the engine
    engine->handleMouseButtonEvent( button, action );
}