#version 410 core

// Input vertex attribute from the VAO
layout (location = 0) in vec3 vPos;
// If you want normals or other attributes, declare them similarly
// layout (location = 1) in vec3 vNorm; // if needed

// Uniforms passed from CPU
uniform mat4 mvpMatrix;

// For direction of motion dependent pattern
out vec3 fragNormal;

void main() {
    // Compute the clip space position of the vertex
    gl_Position = mvpMatrix * vec4(vPos, 1.0);

    fragNormal = (normalMatrix * vec4(vNorm,0.0)).xyz;
}