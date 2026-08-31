#version 460

layout(location = 0) in vec4 vertLocation;
layout(location = 1) in vec4 vertNormal;
layout(location = 2) in vec4 vertTangent;
layout(location = 3) in vec2 vertUv0;

const int MAX_LIGHT_COUNT = 16;

out VS_OUT
{
	vec3 worldLocation;
	vec2 uv0;
	mat3 tbn;
	vec4 worldLocationsLightSpace[MAX_LIGHT_COUNT];
	flat int lightCount;
} vs_out;

uniform mat4 pv;
uniform mat4 model;
uniform mat3 normMatrix;

uniform int lightCount;
uniform mat4 lightSpaceMatrices[MAX_LIGHT_COUNT];

void main()
{
	vs_out.worldLocation = vec3(model * vertLocation);
	vs_out.uv0 = vertUv0;

	vec3 T = normalize(normMatrix * vec3(vertTangent));
	vec3 N = normalize(normMatrix * vec3(vertNormal));
	T = normalize(T - dot(T, N) * N);          // Gram-Schmidt re-orthogonalize, cheap insurance
	vec3 B = cross(N, T) * vertTangent.w;      // apply handedness from the .w component

	vs_out.tbn = transpose(mat3(T, B, N));
	vs_out.lightCount = lightCount;

	// FIX: Loop through MAX_LIGHT_COUNT to ensure the entire array is initialized
	for(int i = 0; i < MAX_LIGHT_COUNT; ++i)
	{
		vs_out.worldLocationsLightSpace[i] = lightSpaceMatrices[i] * vec4(vs_out.worldLocation, 1.0);
	}

	gl_Position = pv * model * vertLocation;
}