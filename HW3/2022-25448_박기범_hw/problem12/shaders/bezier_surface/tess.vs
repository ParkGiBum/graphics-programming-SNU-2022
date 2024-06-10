#version 410
layout(location = 0) in vec3 position;

uniform mat4 model;

void main()
{
	gl_Position = model * vec4(position, 1.0f);//notice that we don't need gl_Position
	//this is because the final position is calculated later
}