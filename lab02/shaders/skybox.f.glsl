#version 410 core

// Input from vertex shader
in vec2 TexCoords;

// Output to frame buffer
out vec4 FragColor;

// Uniforms
uniform sampler2D skyboxTexture;

void main( ) { FragColor = texture( skyboxTexture, TexCoords ); }