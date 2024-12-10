#version 410 core

layout (location = 0) in vec3 vPos;

uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;

out vec3 fragWorldPos;

void main() {
    // Compute world position of this vertex
    vec4 worldPos = modelMatrix * vec4(vPos, 1.0);
    fragWorldPos = worldPos.xyz;

    gl_Position = mvpMatrix * vec4(vPos, 1.0);
}