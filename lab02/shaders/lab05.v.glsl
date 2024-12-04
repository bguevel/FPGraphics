#version 410 core

// Uniform inputs
uniform mat4 mvpMatrix;
uniform mat3 mtxNormal;
uniform mat4 modelMatrix;

// Spot light uniforms
uniform vec3 spotLightPosition;
uniform vec3 spotLightDirection;
uniform float spotLightCutoff;
uniform float spotLightOuterCutoff;

// Camera position
uniform vec3 cameraPosition;

// Light uniforms
uniform vec3 light_direction;
uniform vec3 light_color;

// Material color
uniform vec3 materialColor;

// Attribute inputs
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vertexnorm;

// Varying outputs
layout(location = 0) out vec3 color;

void main() {
    // Transform vertex position to clip space
    gl_Position = mvpMatrix * vec4(vPos, 1.0);

    // Transform position to world space
    vec3 worldPos = vec3(modelMatrix * vec4(vPos, 1.0));

    // Transform normal to world space and normalize
    vec3 transformedNormal = normalize(mtxNormal * vertexnorm);

    // Compute the vector from the light to the vertex
    vec3 lightToVertex = normalize(spotLightPosition - worldPos);

    // Compute the angle between the spotlight direction and the light-to-vertex vector
    float myTheta = dot(lightToVertex, normalize(-spotLightDirection));

    // Compute edge effects
    float epsilon = spotLightCutoff - spotLightOuterCutoff;
    float spotIntensity = clamp((myTheta - spotLightOuterCutoff) / epsilon, 0.0, 1.0);

    // Compute distance and attenuation
    float distance = length(spotLightPosition - worldPos);
    float attenuation = 1.0 / (1.0 + 0.00005 * distance + 0.0001 * distance * distance);

    // Compute eye direction
    vec3 eyeDirection = normalize(cameraPosition - worldPos);

    // Directional light direction (should be normalized)
    vec3 lightVector = normalize(-light_direction);

    // Ambient component
    vec3 ambient = 0.6 * light_color * materialColor;

    // Diffuse component for directional light
    float diffuseFactor = max(dot(transformedNormal, lightVector), 0.0);
    vec3 diffuse = diffuseFactor * light_color * materialColor;

    // Specular component for directional light
    vec3 reflection = reflect(-lightVector, transformedNormal);
    float shininess = 20.0;
    float specularFactor = pow(max(dot(reflection, eyeDirection), 0.0), shininess);
    vec3 specular = specularFactor * light_color;

    // Initial color from directional light
    vec3 finalColor = ambient + diffuse + specular;

    // Spotlight diffuse component
    float diffuseFactor2 = max(dot(transformedNormal, lightToVertex), 0.0);
    vec3 diffuse2 = diffuseFactor2 * vec3(1.0, 1.0, 1.0) * materialColor;

    // Spotlight specular component
    vec3 reflection2 = reflect(-lightToVertex, transformedNormal);
    float shininess2 = 16.0;
    float specularFactor2 = pow(max(dot(reflection2, eyeDirection), 0.0), shininess2);
    vec3 specular2 = specularFactor2 * vec3(1.0, 1.0, 1.0);

    // Apply spotlight intensity and attenuation
    if (myTheta >= spotLightOuterCutoff) {
        spotIntensity *= attenuation;
        diffuse2  *= spotIntensity*20;
        specular2 *= spotIntensity*20;
    } else {
        diffuse2  = vec3(0.0, 0.0, 0.0);
        specular2 = vec3(0.0, 0.0, 0.0);
    }

    // Accumulate spotlight contributions
    finalColor += diffuse2 + specular2;

    // Output the final color
    color = finalColor;
}
