#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform vec4 vertexColor;
uniform mat4 transform;

out vec3 ourColor;
out vec3 ourPos;


void main()
{
    gl_Position =transform * vec4(aPos, 1.0);
}