#version 410 core

in vec3 fragNormal;

uniform float cartSpeed;
uniform vec3 materialColor;

out vec4 fragColor;

void main() {
    vec4 baseColor = vec4(materialColor, 1.0);

    float speedThreshold = 2.0; // adjust as needed

    // Normalize the fragment normal
    vec3 N = normalize(fragNormal);

    // Use fragNormal to rotate the line pattern.

    float angle = atan(N.y, N.x);

    // Rotate our coordinate system by this angle.

    float rotatedX = gl_FragCoord.x * cos(angle) - gl_FragCoord.y * sin(angle);
    // Now rotatedX is shifted according to the normal direction, which can change line orientation

    if (cartSpeed > speedThreshold) {
        // Create a line pattern using the rotated coordinate
        float frequency = 0.1; // how dense the lines are
        float linePattern = fract(rotatedX * frequency);
        float mask = step(0.5, linePattern);

        // blend white stripes
        vec4 lineColor = vec4(1.0, 1.0, 1.0, 1.0);
        baseColor = mix(baseColor, lineColor, mask * 0.3);
    }

    fragColor = baseColor;
}