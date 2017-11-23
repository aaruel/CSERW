#version 410 core

in vec3 position;
in vec2 UV_coordinates;
out vec2 Uvs;

uniform mat4 MVP;

void main()
{
    Uvs = UV_coordinates;
    gl_Position = MVP*vec4(position, 1.0);
}