// height.v.glsl
#version 410 core

// Uniforms
uniform mat4 mvpMatrix;          // Model-View-Projection matrix
uniform mat4 modelMatrix;        // Model matrix
uniform mat3 normalMatrix;       // Normal matrix (transpose of inverse of model matrix)
uniform sampler2D heightMap;     // Height map texture
uniform float maxHeight;         // Maximum height displacement
uniform vec2 texelSize;          // Size of one texel (1.0 / texture dimensions)
uniform vec3 cameraPosition;     // Camera position in world space

// Attributes
layout(location = 0) in vec3 vPos;      // Vertex position
layout(location = 1) in vec3 vNormal;   // Vertex normal
layout(location = 2) in vec2 vTexCoord; // Vertex texture coordinate

// Varyings
out vec3 fragPosition;    // World-space position of the fragment
out vec3 fragNormal;      // World-space normal of the fragment
out vec3 viewDir;         // Direction from fragment to camera

void main() {
    // Sample height from the height map using texture coordinates
    float height = texture(heightMap, vTexCoord).r * maxHeight;


    // Apply displacement to the vertex position along the normal
    vec3 displacedPos = vPos + vNormal * height;

    // Transform the displaced position to world space
    vec4 worldPos = modelMatrix * vec4(displacedPos, 1.0);

    // Pass the world-space position to the fragment shader
    fragPosition = vec3(worldPos);

    // Compute gradients in the x and z directions
    float heightL = texture(heightMap, vTexCoord + vec2(-texelSize.x, 0.0)).r * maxHeight;
    float heightR = texture(heightMap, vTexCoord + vec2(texelSize.x, 0.0)).r * maxHeight;
    float heightD = texture(heightMap, vTexCoord + vec2(0.0, -texelSize.y)).r * maxHeight;
    float heightU = texture(heightMap, vTexCoord + vec2(0.0, texelSize.y)).r * maxHeight;

    // Calculate the derivative of the height map
    float dx = heightR - heightL;
    float dz = heightU - heightD;

    // Compute the normal based on the gradients
    vec3 computedNormal = normalize(vec3(-dx, 2.0, -dz));

    // Transform the normal to world space using the normal matrix
    fragNormal = normalize(normalMatrix * computedNormal);

    // Compute the view direction (from fragment to camera)
    viewDir = normalize(cameraPosition - fragPosition);

    // Compute the final vertex position in clip space
    gl_Position = mvpMatrix * worldPos;
}
