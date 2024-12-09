#ifndef LAB02_LAB02_ENGINE_H
#define LAB02_LAB02_ENGINE_H

#include "FreeCam.h"
#include <CSCI441/FreeCam.hpp>
#include <CSCI441/OpenGLEngine.hpp>
#include <CSCI441/ShaderProgram.hpp>
#include <glm/glm.hpp>
#include "Being.h"
#include "Car.h"
#include <vector>

class Lab02Engine final : public CSCI441::OpenGLEngine {
public:
 void _moveMarbles();
 void _collideMarblesWithMarbles();
    Lab02Engine();
    ~Lab02Engine() final;
 /// \desc value off-screen to represent mouse has not begun interacting with window yet
 static constexpr GLfloat MOUSE_UNINITIALIZED = -9999.0f;
 void handleKeyEvent(GLint key, GLint action);

 /// \desc handle any mouse button events inside the engine
 /// \param button mouse button as represented by GLFW_MOUSE_BUTTON_ macros
 /// \param action mouse event as represented by GLFW_ macros
 void handleMouseButtonEvent(GLint button, GLint action);

 /// \desc handle any cursor movement events inside the engine
 /// \param currMousePosition the current cursor position
 void handleCursorPositionEvent(glm::vec2 currMousePosition);
    void run() final;

    /**
     * @brief returns a pointer to our free cam object
     */
    [[nodiscard]] FreeCam* getFreeCam() const { return _pFreeCam; }

    /**
     * @brief returns the last location of the mouse in window coordinates
     * @return last location of mouse in window coordinates
     */
    [[nodiscard]] glm::vec2 getMousePosition() const { return _mousePosition; }
    /**
     * @brief updates the last location of the mouse
     * @param mousePos new mouse location in window coordinates
     */
    void setMousePosition(glm::vec2 mousePos) { _mousePosition = mousePos; }

    /**
     * @brief returns the current state of the left mouse button
     * @return current left mouse button state
     */
    [[nodiscard]] GLint getLeftMouseButtonState() const { return _leftMouseButtonState; }
    /**
     * @brief updates the current state of the left mouse button
     * @param state current left mouse button state
     */
    void setLeftMouseButtonState(GLint state) { _leftMouseButtonState = state; }

    /// \desc value off-screen to represent mouse has not begun interacting with window yet
    /// \desc speed of camera movement forwards/backwards
    static constexpr GLfloat CAMERA_MOVEMENT = 1.5f;    // because the direction vector is unit length, and we probably don't want
                                                        // to move one full unit every time a button is pressed, just create a constant
                                                        // to keep track of how far we want to move at each step. you could make
                                                        // this change w.r.t. the amount of time the button's held down for
                                                        // simple scale-sensitive movement!



 std::vector<float> heightData;
 int heightMapWidth, heightMapHeight, heightMapChannels;
 float maxHeight = 10.0f; // Adjust as needed

 bool loadHeightMap(const std::string& filepath);
 void generateTerrainMesh();


 const float WORLD_BOUNDARY = 100.0f; // Adjust based on your world size
 const float CORNER_REGION_SIZE = 10.0f; // Size of the corner regions
 int _numCornersVisited = 0;
 void _checkForCornerVisits();
protected:
    void mSetupGLFW() final;
    void mSetupOpenGL() final;
    void mSetupShaders() final;
    void mSetupBuffers() final;
    void mSetupScene() final;
 void calculateTerrainNormals();
 float getTerrainHeight(float x, float z) const;
 GLuint _loadAndRegisterSkyboxTexture( const char* FILENAME );
 void _createSkyboxBuffers( );

private:
 GLint _skyTex;
 CSCI441::ShaderProgram* _skyboxShaderProgram;

 struct SkyboxShaderUniformLocations
 {
  GLint modelMatrix;
  GLint viewMatrix;
  GLint projectionMatrix;
  GLint skyboxTexture;
 } _skyboxShaderUniformLocations;

 GLuint _skyboxVAO, _skyboxVBO;

 float _centerX;
 float _centerZ;
 CSCI441::ShaderProgram* _terrainShaderProgram = nullptr;

 // Uniform and attribute locations for the terrain shader

 GLuint _heightMapTextureID;
 struct TerrainShaderUniformLocations {
  GLint mvpMatrix;
  GLint modelMatrix;
  GLint normalMatrix;
  GLint heightMap;
  GLint maxHeight;
  GLint materialColor;
  GLint camPosition;
  GLint lightColor;
  GLint lightPosition;
  GLint spotLightPosition;
  GLint spotLightDirection;
  GLint spotLightCutoff;
  GLint spotLightOuterCutoff;
  GLint texelSize; // Add this line
  // Add any other uniforms if needed
 } _terrainShaderUniformLocations;

 struct TerrainShaderAttributeLocations {
  GLint vPos;
  GLint vNormal;
  GLint vTexCoord;
  // Add any other attributes if needed
 } _terrainShaderAttributeLocations;
 GLuint _heightMapTexture;
 GLuint _terrainVAO, _terrainVBO, _terrainIBO;
 std::vector<float> terrainVertices;
 std::vector<unsigned int> terrainIndices;
 int moveNose =0;

 /**
  * @brief information list of all the buildings to draw
  */
 struct LightingShaderUniformLocations {
  /// \desc precomputed MVP matrix location
  GLint mvpMatrix;
  /// \desc material diffuse color location
  GLint materialDiffuse;
  GLint materialSpecular;
  GLint materialShine;
  GLint modelMatrix;
  // TODO #1: add new uniforms
  GLint vNormalMatrix;
  GLint lightPosition;
  GLint lightColor;
  GLint ViewMtx;
  GLint camPosition;

  GLint spotLightPosition;
  GLint spotLightDirection;
  GLint spotLightCutoff;
  GLint spotLightOuterCutoff;

 } _lightingShaderUniformLocations;
 struct LightingShaderAttributeLocations {
  /// \desc vertex position location
  GLint vPos;
  // TODO #2: add new attributes
  GLint vNorm;

 } _lightingShaderAttributeLocations;
 CSCI441::ShaderProgram* _lightingShaderProgram = nullptr;
 Car* _pPlayerCar;
    /**
     * @brief Creates a VAO to draw a simple grid environment with buildings for the user to navigate through.
     */
    void _generateEnvironment();
 void _computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
    void _renderScene(glm::mat4 viewMtx, glm::mat4 projMtx) const;
    void _updateScene();

    /**
     * @brief VAO representing our ground plane grid
     */
    GLuint _gridVAO;
    /**
     * @brief tracks the number of vertices comprising the ground plane
     */
    GLuint _numGridPoints;
    /**
     * @brief color to draw the ground plane grid
     */
    glm::vec3 _gridColor;

    /**
     * @brief pointer to the camera in our world set up as a FreeCam
     */
    FreeCam* _pFreeCam;

    /**
     * @brief last location of the mouse in window coordinates
     */
    glm::vec2 _mousePosition;
    /**
     * @brief current state of the left mouse button
     */
    GLint _leftMouseButtonState;
 /// \desc tracks the number of different keys that can be present as determined by GLFW
 static constexpr GLuint NUM_KEYS = GLFW_KEY_LAST;
 /// \desc boolean array tracking each key state.  if true, then the key is in a pressed or held
 /// down state.  if false, then the key is in a released state and not being interacted with
 GLboolean _keys[NUM_KEYS];
 /// \desc pair of values to store the speed the camera can move/rotate.
 /// \brief x = forward/backward delta, y = rotational delta
 glm::vec2 _cameraSpeed;
 /// \desc the size of the world (controls the ground size and locations of buildings)
 static constexpr GLfloat WORLD_SIZE = 100.0f;
 /// \desc VAO for our ground
 GLuint _groundVAO;
 /// \desc the number of points that make up our ground object
 GLsizei _numGroundPoints;
 void _createGroundBuffers();

};

void lab02_engine_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods );
void lab02_engine_cursor_callback(GLFWwindow *window, double x, double y );
void lab02_engine_mouse_button_callback(GLFWwindow *window, int button, int action, int mods );

#endif // LAB02_LAB02_ENGINE_H