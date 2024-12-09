#version 410 core

// uniform inputs
// Material Uniforms
uniform vec4 materialDiffuse;
uniform vec4 materialSpecular;
uniform float materialShine;
uniform int isEmitter;

// Spot light uniforms
uniform vec3 spotLightPosition;
uniform vec3 spotLightDirection;
uniform float spotLightCutoff;
uniform float spotLightOuterCutoff;

uniform vec3 light_direction;
uniform vec3 light_color;

// varying inputs
//layout(location = 0) in vec3 color;     // interpolated color for this fragment
in vec3 fragNormal;            // World-space normal of the fragment
in vec3 fragPosition;          // World-space position of the fragment
in vec3 viewDir;               // Direction from fragment to camera

// outputs
out vec4 fragColorOut;                  // color to apply to this fragment

void main() {
    if (isEmitter == 1) {
        fragColorOut = materialSpecular;
        return;
    }
    // Normalize the input normal vector
    vec3 normalizedNormal = normalize(fragNormal);

    // Normalize the directional light direction vector
    vec3 normalizedLightDir = normalize(-light_direction); // Assuming light_direction points *to* the light

    // Compute the diffuse intensity using Lambert's cosine law for directional light
    float diffIntensity = max(dot(normalizedNormal, normalizedLightDir), 0.0);

    // Calculate the diffuse color contribution from directional light
    vec3 diffuse = diffIntensity * light_color * vec3(materialDiffuse);

    // Ambient component
    vec3 ambient = 0.2 * light_color * vec3(materialDiffuse); // Reduced ambient to prevent over-brightness

    // Specular component for directional light
    vec3 reflectDir = reflect(-normalizedLightDir, normalizedNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShine);
    vec3 specular = light_color * spec * light_color;

    // Initial color from directional light
    vec3 finalColor = ambient + diffuse + specular;

    // --- Spotlight Calculations ---

    // Compute the vector from the fragment to the spotlight
    //vec3 lightToFrag = normalize(fragPosition - spotLightPosition);
    vec3 lightToFrag = normalize(spotLightPosition - fragPosition);

    // Calculate the angle between the spotlight direction and the fragment direction
    //float theta = dot(lightToFrag, normalize(spotLightDirection));
    vec3 normalizedSpotDir = normalize(spotLightDirection);
    float theta = dot(normalizedSpotDir, lightToFrag);

    // Determine if the fragment is inside the spotlight cone
    float epsilon = spotLightCutoff - spotLightOuterCutoff;
    float intensity = clamp((spotLightOuterCutoff -theta) / epsilon, 0.0, 1.0);

    // Compute distance for attenuation
    float distance = length(spotLightPosition - fragPosition);
    float attenuation = 1.0 / (1.0 + 0.00009 * distance + 0.000032 * distance * distance);

    // Final spotlight intensity
    float spotIntensity = intensity * attenuation;

    if(theta <= spotLightOuterCutoff) {
        // Diffuse component for spotlight
        float diffSpot = max(dot(normalizedNormal, lightToFrag), 0.0);
        vec3 diffuseSpot =  light_color * vec3(materialDiffuse) * diffSpot *spotIntensity*5;

        // Specular component for spotlight
        vec3 reflectSpot = reflect(-lightToFrag, normalizedNormal);
        float specSpot = pow(max(dot(viewDir, reflectSpot), 0.0), materialShine);
        vec3 specularSpot = light_color * specSpot * spotIntensity * vec3(materialSpecular);

        // Add spotlight contributions with intensity
        finalColor += diffuseSpot + specularSpot;
    }

    // Ensure final color doesn't exceed 1.0
    finalColor = clamp(finalColor, 0.0, 1.0);

    // Set the fragment color
    fragColorOut = vec4(finalColor, 1.0);
}