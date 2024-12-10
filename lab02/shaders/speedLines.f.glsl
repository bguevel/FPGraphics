#version 410 core

in vec3 fragWorldPos;

uniform float cartSpeed;
uniform vec3 materialColor;
uniform vec3 cartPosition;
uniform vec3 cartDirection;

out vec4 fragColor;

void main() {
    vec4 baseColor = vec4(materialColor, 1.0);
    float speedThreshold = 3.0; // adjust as needed

    if (cartSpeed > speedThreshold) {
        // Compute the vector from the cart to this fragment
        vec3 toFrag = fragWorldPos - cartPosition;

        // Project this onto the cartDirection to get a distance along direction
        float distAlongDir = dot(toFrag, normalize(cartDirection));

        // frequency of lines
        float frequency = 0.5; // adjust for spacing of lines
        // create a repeating pattern along direction axis
        float linePattern = fract(distAlongDir * frequency);

        // Use step to create stripes
        float mask = step(0.5, linePattern);
        vec4 lineColor = vec4(1.0, 1.0, 1.0, 1.0);

        // Mix the base color with lineColor based on mask
        // This creates stripes along the cart's direction
        baseColor = mix(baseColor, lineColor, mask * 0.3);
    }

    fragColor = baseColor;
}