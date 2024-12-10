// height.f.glsl
#version 410 core

// Uniforms
uniform vec3 materialColor;         // Base color of the terrain
uniform vec3 light_color;           // Color/intensity of the directional light
uniform vec3 light_direction;       // Direction of the directional light (should be normalized)

// Spotlight uniforms
uniform vec3 spotLightPosition;     // Position of the spotlight in world space
uniform vec3 spotLightDirection;    // Direction of the spotlight (should be normalized)
uniform float spotLightCutoff;      // Cosine of the inner cutoff angle
uniform float spotLightOuterCutoff; // Cosine of the outer cutoff angle

uniform vec3 cameraPosition;        // Camera position in world space

uniform sampler2D trackFilter;     // Height map texture
uniform sampler2D trackTexture;
uniform sampler2D sceneTexture;

uniform sampler2D eggTexture;
uniform int isReverse;


// Inputs from Vertex Shader
in vec3 fragNormal;            // World-space normal of the fragment
in vec3 fragPosition;          // World-space position of the fragment
in vec3 viewDir;               // Direction from fragment to camera
in vec2 fTCoord;                //TextureCoord

// Output
out vec4 fragColor;

void main() {

    vec3 color;
    if (float(texture(trackFilter, fTCoord)) != 1.0f) {
        if (isReverse == 0) color = vec3(texture(trackTexture, fTCoord));
        else {
            //color = vec3(sin(fragPosition.x), sin(fragPosition.y), sin(fragPosition.z) );
            color = vec3(texture(eggTexture, fTCoord));
        }
    }
    else color = vec3(texture(sceneTexture, fTCoord));

    // Normalize the input normal vector
    vec3 normalizedNormal = normalize(fragNormal);

    // Normalize the directional light direction vector
    vec3 normalizedLightDir = normalize(-light_direction); // Assuming light_direction points *to* the light

    // Compute the diffuse intensity using Lambert's cosine law for directional light
    float diffIntensity = max(dot(normalizedNormal, normalizedLightDir), 0.0);

    // Calculate the diffuse color contribution from directional light
    vec3 diffuse = diffIntensity * light_color * color;

    // Ambient component
    vec3 ambient = 0.2 * light_color * color; // Reduced ambient to prevent over-brightness

    // Specular component for directional light
    vec3 reflectDir = reflect(-normalizedLightDir, normalizedNormal);
    float specStrength = 0.5;
    float shininess = 32.0;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specStrength * spec * light_color;

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
    float attenuation = 1.0 / (1.0 + 0.009 * distance + 0.0032 * distance * distance);

    // Final spotlight intensity
    float spotIntensity = intensity * attenuation;

    if(theta <= spotLightOuterCutoff) {
        // Diffuse component for spotlight
        float diffSpot = max(dot(normalizedNormal, lightToFrag), 0.0);
        vec3 diffuseSpot =  light_color * color * diffSpot *spotIntensity*5;

        // Specular component for spotlight
        vec3 reflectSpot = reflect(-lightToFrag, normalizedNormal);
        float specSpot = pow(max(dot(viewDir, reflectSpot), 0.0), shininess);
        vec3 specularSpot = specStrength * light_color * specSpot *spotIntensity*5;

        // Add spotlight contributions with intensity
        finalColor += diffuseSpot + specularSpot;
    }

    // Ensure final color doesn't exceed 1.0
    finalColor = clamp(finalColor, 0.0, 1.0);

    // Set the fragment color
    fragColor = vec4(finalColor, 1.0);
    //fragColor = vec4(vec3(spotIntensity,spotIntensity,spotIntensity),1.0);


}
