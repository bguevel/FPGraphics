#version 410 core

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 mvpMatrix;

void main() {
    // Thickness of the speed lines
    float thickness = 0.4;

    // Get the two vertices of the line
    vec4 p0 = gl_in[0].gl_Position;
    vec4 p1 = gl_in[1].gl_Position;

    // Compute the direction and perpendicular vector
    vec3 dir = normalize(p1.xyz - p0.xyz);
    vec3 perpendicular = normalize(vec3(-dir.y, dir.x, 0.0)) * thickness;

    // Emit two triangles forming a quad
    gl_Position = p0 + vec4(perpendicular, 0.0);
    EmitVertex();

    gl_Position = p1 + vec4(perpendicular, 0.0);
    EmitVertex();

    gl_Position = p0 - vec4(perpendicular, 0.0);
    EmitVertex();

    gl_Position = p1 - vec4(perpendicular, 0.0);
    EmitVertex();

    EndPrimitive();
}