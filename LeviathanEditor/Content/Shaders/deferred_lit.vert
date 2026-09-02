#version 460

layout(location = 0) in vec4 vertLocation;
layout(location = 3) in vec2 vertUv0;

uniform int lightCount;

out VS_OUT
{
	vec2 uv0;
	flat int lightCount;
} vs_out;

void main()
{
	vs_out.lightCount = lightCount;

    vs_out.uv0 = vertUv0;
    gl_Position = vertLocation;
}