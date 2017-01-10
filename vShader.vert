#version 410 core

in vec3 position;
in vec3 color;
out vec3 Color;

uniform mat4 MVP;
//uniform mat4 M;
//uniform mat4 V;
//uniform mat4 P;


void main()
{
    Color = color;
    gl_Position = MVP*vec4(position, 1.0);
}