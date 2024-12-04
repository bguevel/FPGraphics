#version 410 core

// Uniforms
uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;
uniform sampler2D heightMap;
uniform float maxHeight;

// Attributes
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexCoord;

// Varyings
out vec2 fragTexCoord;
out vec3 fragPosition;

void main() {
    // Sample height from the heightmap texture
    float height = texture(heightMap, vTexCoord).r * maxHeight;
    vec3 displacedPosition = vec3(vPos.x, height, vPos.z);

    gl_Position = mvpMatrix * vec4(displacedPosition, 1.0);

    fragTexCoord = vTexCoord;
    fragPosition = vec3(modelMatrix * vec4(displacedPosition, 1.0));
}
