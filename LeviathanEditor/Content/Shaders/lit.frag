#version 460

const int MAX_LIGHT_COUNT = 16;

in VS_OUT
{
	vec3 worldLocation;
	vec2 uv0;
	mat3 tbn;
	vec4 worldLocationsLightSpace[MAX_LIGHT_COUNT];
	float lightCount;
} fs_in;

struct Material
{
	vec3 tint;
	float specularStrength;
	sampler2D baseColor;
	sampler2D normalMap;
	sampler2D ormMap;
};

struct Light
{
	int type;
	vec3 direction;
	vec3 location;
	vec3 color;
};

struct Shadows
{
	sampler2D map;
	float bias;
	float texelSize;
	int samples;
};

uniform Material material;

uniform Light lights[MAX_LIGHT_COUNT];

uniform Shadows shadows;

uniform vec3 cameraLocation;

out vec4 fragColor;

float ShadowCalculation(vec4 fragPosLightSpace, float bias)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;

	float closestDepth = texture(shadows.map, projCoords.xy).r;
	float currentDepth = projCoords.z;

	float shadow = 0.0;
	if(projCoords.z <= 1.0)
	{
		float iterationCount = 0.0;
		vec2 texelSize = shadows.texelSize / textureSize(shadows.map, 0);
		for(int x = -shadows.samples; x <= shadows.samples; ++x)
		{
			for(int y = -shadows.samples; y <= shadows.samples; ++y)
			{
				iterationCount += 1.0;
				float pcfDepth = texture(shadows.map, projCoords.xy + vec2(x, y) * texelSize).r;
				shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
			}
		}

		shadow /= iterationCount;
	}

	return shadow;
}

void main()
{
	vec3 normSample = texture2D(material.normalMap, fs_in.uv0).rgb;
	vec4 baseColor = texture2D(material.baseColor, fs_in.uv0);

	vec3 norm = normSample * 2.0 - 1.0;

	vec3 lightDir = fs_in.tbn * normalize(lights[0].location - fs_in.worldLocation);
	vec3 viewDir = fs_in.tbn * normalize(cameraLocation - fs_in.worldLocation);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	float spec = pow(max(dot(norm, halfwayDir), 0.0), 64.0);
	vec3 specular = lights[0].color * material.specularStrength * spec;

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = lights[0].color * baseColor.rgb * diff;

	float ambientStrength = 0.1;
	vec3 ambient = lights[0].color * ambientStrength;

	float bias = max(0.05 * (1.0 - dot(norm, lightDir)), shadows.bias);
	
	float shadow = 0.0;
	for(int i = 0; i < fs_in.lightCount; ++i)
	{
		shadow += ShadowCalculation(fs_in.worldLocationsLightSpace[i], bias);
	}

	if(fs_in.lightCount > 1)
	{
		shadow /= fs_in.lightCount; // normalize the shadow value
	}

	vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * material.tint;

	fragColor = vec4(result, 1.0);
}