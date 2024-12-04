#version 410 core

// uniform inputs
uniform mat4 mvpMatrix;                 // the precomputed Model-View-Projection Matrix
// TODO #D: add normal matrix
uniform mat3 mtxNormal; //should this be a uniform
uniform mat4 modelMatrix;

// Spot light uniforms
uniform vec3 spotLightPosition;
uniform vec3 spotLightDirection;

uniform float spotLightCutoff;      // Cosine of the inner cutoff angle
uniform float spotLightOuterCutoff; // Cosine of the outer cutoff angle

//uniform vec3 cameraDirection;
uniform vec3 cameraPosition;


// TODO #A: add light uniforms
uniform vec3 light_direction;
uniform vec3 light_color;


uniform vec3 materialColor;             // the material color for our vertex (& whole object)

// attribute inputs
layout(location = 0) in vec3 vPos;      // the position of this specific vertex in object space
// TODO #C: add vertex normal
layout(location = 1) in vec3 vertexnorm;

// varying outputs
layout(location = 0) out vec3 color;    // color to apply to this vertex


void main() {
    // Transform & output the vertex in clip space
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
    float attenuation = 1.0 / (0.05 + 0.09 * distance + 0.032 * distance * distance);

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

    // Initialize color with directional light components
    vec3 color = ambient + diffuse + specular;

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
        diffuse2  *= spotIntensity;
        specular2 *= spotIntensity;
    } else {
        diffuse2  = vec3(0.0, 0.0, 0.0);
        specular2 = vec3(0.0, 0.0, 0.0);
    }

    // Add spotlight contributions to the color
    color = diffuse2 + specular2 + ambient;

}