#version 460

layout(location = 0) in vec4 vertLocation;

void main()
{
	gl_Position = vertLocation;
}