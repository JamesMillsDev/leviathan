#version 460

in VS_OUT
{
	vec3 worldLocation;
	vec2 uv0;
	mat3 tbn;
} fs_in;

struct Material
{
	vec3 tint;
	float specularStrength;
	sampler2D baseColor;
	sampler2D normalMap;
};

struct Light
{
	int type;
	vec3 direction;
	vec3 location;
	vec3 color;
};

uniform Material material;

const int MAX_LIGHT_COUNT = 16;
uniform int lightCount;
uniform Light lights[MAX_LIGHT_COUNT];

uniform vec3 cameraLocation;

out vec4 fragColor;

void main()
{
	vec3 normSample = texture2D(material.normalMap, fs_in.uv0).rgb;
	vec4 baseColor = texture2D(material.baseColor, fs_in.uv0);

	vec3 norm = normSample * 2.0 - 1.0;

	vec3 lightDir = fs_in.tbn * normalize(lights[0].location - fs_in.worldLocation);
	vec3 viewDir = fs_in.tbn * normalize(cameraLocation - fs_in.worldLocation);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
	vec3 specular = lights[0].color * material.specularStrength * spec;

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = lights[0].color * baseColor.rgb * diff;

	float ambientStrength = 0.1;
	vec3 ambient = lights[0].color * ambientStrength;

	vec3 result = (ambient + diffuse + specular) * material.tint;
	fragColor = vec4(result, 1.0);
}