#version 460 

const int MAX_LIGHT_COUNT = 16;

in VS_OUT
{
	vec2 uv0;
	flat int lightCount;
} fs_in;

struct GBuffer
{
    sampler2D location;
    sampler2D normal;
    sampler2D tangent;
    sampler2D albedo;
    sampler2D tint;
    sampler2D orm;
    int debugPhase; 
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

uniform mat4 lightSpaceMatrices[MAX_LIGHT_COUNT];
uniform Light lights[MAX_LIGHT_COUNT];
uniform Shadows shadows;
uniform GBuffer gBuffer;
uniform vec3 cameraLocation;

vec4 worldLocationsLightSpace[MAX_LIGHT_COUNT];

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

void ComputeWorldSpaceLightLocations()
{
    vec4 location = texture(gBuffer.location, fs_in.uv0);

	for(int i = 0; i < MAX_LIGHT_COUNT; ++i)
	{
		worldLocationsLightSpace[i] = lightSpaceMatrices[i] * location;
	}
}

void main()
{
    if(gBuffer.debugPhase == 1) { fragColor = vec4(texture(gBuffer.location, fs_in.uv0).rgb, 1.0); return; }
    if(gBuffer.debugPhase == 2) { fragColor = vec4(texture(gBuffer.normal, fs_in.uv0).rgb, 1.0); return; }
    if(gBuffer.debugPhase == 3) { fragColor = vec4(texture(gBuffer.tangent, fs_in.uv0).rgb, 1.0); return; }
    if(gBuffer.debugPhase == 4) { fragColor = vec4(texture(gBuffer.albedo, fs_in.uv0).rgb, 1.0); return; }
    if(gBuffer.debugPhase == 5) { fragColor = vec4(texture(gBuffer.tint, fs_in.uv0).rgb, 1.0); return; }
    if(gBuffer.debugPhase == 6) { fragColor = vec4(texture(gBuffer.orm, fs_in.uv0).rgb, 1.0); return; }

    ComputeWorldSpaceLightLocations();

    vec3 location = texture(gBuffer.location, fs_in.uv0).rgb;
    vec3 normal = normalize(texture(gBuffer.normal, fs_in.uv0).rgb);
    vec4 baseColor = texture(gBuffer.albedo, fs_in.uv0) * texture(gBuffer.tint, fs_in.uv0);
    vec4 orm = texture(gBuffer.orm, fs_in.uv0);

    float roughness = orm.g; 
	float maxShininess = 256.0; 
    float shininess = pow(maxShininess, 1.0 - max(roughness, 0.001));

    vec3 ambient = vec3(0.1) * baseColor.rgb;
    vec3 totalDiffuseSpecular = vec3(0.0);
    vec3 viewDir = normalize(cameraLocation - location);


	// Loop strictly up to the active light count
	for(int i = 0; i < fs_in.lightCount; ++i)
	{
		// Since they are directional lights, light location acts as the target position vector
		vec3 lightDir = normalize(-lights[i].direction);
		vec3 halfwayDir = normalize(lightDir + viewDir);

		// Calculate shadow for this specific light
		float bias = max(0.05 * (1.0 - dot(normal, lightDir)), shadows.bias);
		float shadow = ShadowCalculation(worldLocationsLightSpace[i], bias);

		float diff = max(dot(normal, lightDir), 0.0);
		vec3 diffuse = lights[i].color * baseColor.rgb * diff;

		float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
		spec *= step(0.0, dot(normal, lightDir));   // gate specular on the same test as diffuse
		vec3 specular = lights[i].color * spec;

		// Accumulate light calculations independently
		totalDiffuseSpecular += (1.0 - shadow) * (diffuse + specular);
	}

	vec3 result = ambient + totalDiffuseSpecular;
	fragColor = vec4(result, 1.0);
}