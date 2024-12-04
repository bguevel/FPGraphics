// diffuse_only.f.glsl
#version 410 core

// Uniforms
uniform vec3 materialColor;     // Base color of the terrain
uniform vec3 light_color;        // Color/intensity of the directional light
uniform vec3 light_direction;    // Direction of the directional light (should be normalized)

// Inputs from Vertex Shader
in vec3 fragNormal;            // World-space normal of the fragment
in vec3 fragPosition;          // World-space position of the fragment (optional for diffuse)

// Output
out vec4 fragColor;

void main() {
    // Normalize the input normal vector
    vec3 normalizedNormal = normalize(fragNormal);

    // Normalize the light direction vector
    vec3 normalizedLightDir = normalize(-light_direction); // Assuming lightDirection points *to* the light

    // Compute the diffuse intensity using Lambert's cosine law
    float diffIntensity = max(dot(normalizedNormal, normalizedLightDir), 0.0);

    // Calculate the diffuse color contribution
    vec3 diffuse = diffIntensity * light_color * materialColor;
    vec3 ambient = 0.5 * light_color * materialColor;

    // Set the fragment color
    fragColor = vec4(diffuse+ambient, 1.0);

    vec3 color = normalize(fragNormal) * 0.5 + 0.5; // Map normals from [-1,1] to [0,1]
    fragColor = vec4(color, 1.0);

}
