#version 460

layout(location = 0) in vec4 vertLocation;
layout(location = 1) in vec4 vertNormal;

out VS_OUT
{
	vec3 normal;
	vec3 worldLocation;
} vs_out;

uniform mat4 pv;
uniform mat4 model;

void main()
{
	vs_out.normal = vec3(vertNormal);
	vs_out.worldLocation = vec3(model * vertLocation);

	gl_Position = pv * model * vertLocation;
}