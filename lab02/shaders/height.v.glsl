#version 410 core

// Uniforms
uniform mat4 mvpMatrix;
uniform mat3 normalMatrix;

// Attributes
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;

// Varyings
out vec3 fragNormal;

void main() {
    gl_Position = mvpMatrix * vec4(vPos, 1.0);
    fragNormal = normalize(normalMatrix * vNormal);
}