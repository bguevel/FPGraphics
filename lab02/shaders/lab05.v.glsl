#version 410 core

// uniform inputs
uniform mat4 mvpMatrix;                 // the precomputed Model-View-Projection Matrix
// TODO #D: add normal matrix
uniform mat3 mtxNormal; //should this be a uniform


// TODO #A: add light uniforms
uniform vec3 light_direction;
uniform vec3 light_color;
uniform mat4 view_mtx; // should this be a vec3 for mat4?
uniform vec3 cam_position;

uniform vec3 materialColor;             // the material color for our vertex (& whole object)

// attribute inputs
layout(location = 0) in vec3 vPos;      // the position of this specific vertex in object space
// TODO #C: add vertex normal
in vec3 vertexnorm;

// varying outputs
layout(location = 0) out vec3 color;    // color to apply to this vertex


void main() {
    // transform & output the vertex in clip space
    gl_Position = mvpMatrix * vec4(vPos, 1.0);

    // TODO #B: compute Light vector
    vec3 light_vec = normalize(-light_direction);// how do we compute the light vector

    // TODO #E: transform normal vector
    vec3 transformNorm = normalize(mtxNormal* vertexnorm);
    vec3 spec;
    // TODO #F: perform diffuse calculation
    vec3 diffColor = light_color *materialColor *max(dot(light_vec,transformNorm),0);
    // TODO #G: assign the color for this vertex
    color = diffColor;
    // specular
    vec3 viewing_vector = vec3(vec4(cam_position,1)-(view_mtx*vec4(vPos,1)));
    vec3 reflected = light_vec+2*dot(vertexnorm,light_direction)*vertexnorm;
    spec = materialColor * light_color * pow(max(dot(viewing_vector,reflected),0),0.1); // how do I then do this with a mat4 instead?
    color = color + spec;
    //amb
    vec3 ambient = vec3(0.05,0.05,0.05);
    color = color + ambient;
}