#version 460

layout(location = 0) in vec4 vertLocation;
layout(location = 1) in vec4 vertNormal;
layout(location = 2) in vec4 vertTangent;
layout(location = 3) in vec2 vertUv0;

out VS_OUT
{
	vec3 worldLocation;
	vec2 uv0;
	mat3 tbn;
} vs_out;

uniform mat4 pv;
uniform mat4 model;
uniform mat3 normMatrix;

void main()
{
	vs_out.worldLocation = vec3(model * vertLocation);
	vs_out.uv0 = vertUv0;

	vec3 T = normalize(normMatrix * vec3(vertTangent));
	vec3 N = normalize(normMatrix * vec3(vertNormal));
	vec3 B = normalize(cross(T, N));

	vs_out.tbn = transpose(mat3(T, B, N));

	gl_Position = pv * model * vertLocation;
}