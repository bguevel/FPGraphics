#version 410 core

// Vertex attributes
layout( location = 0 ) in vec3 vPos;
layout( location = 1 ) in vec2 vTexCoords;

// Output to fragment shader
out vec2 TexCoords;

// Uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main( )
{
  TexCoords   = vTexCoords;
  gl_Position = projection * view * model * vec4( vPos, 1.0 );
}