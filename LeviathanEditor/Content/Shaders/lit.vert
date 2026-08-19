#version 460

layout(location = 0) in vec4 vertLocation;

uniform mat4 vp;
uniform mat4 model;

void main()
{
	gl_Position = model * vp * vertLocation;
}