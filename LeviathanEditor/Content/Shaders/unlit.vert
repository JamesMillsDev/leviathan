#version 460

layout(location = 0) in vec4 vertLocation;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vertLocation;
}