#version 410 core

// Varyings
in vec3 fragNormal;

// Outputs
out vec4 fragColor;

void main() {
    // Simple diffuse lighting
    vec3 lightDir = normalize(vec3(-1.0, -1.0, -1.0));
    vec3 color = vec3(0.3, 0.8, 0.3) * max(dot(fragNormal, lightDir), 0.0);
    fragColor = vec4(color, 1.0);
}