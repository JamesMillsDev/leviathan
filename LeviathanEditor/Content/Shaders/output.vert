#version 460 

layout(location = 0) in vec4 vertLocation;
layout(location = 3) in vec2 vertUv0;

out vec2 uv0;

void main()
{
    uv0 = vertUv0;
    gl_Position = vertLocation;
}