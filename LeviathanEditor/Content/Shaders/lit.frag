#version 460

in VS_OUT
{
	vec3 normal;
	vec3 worldLocation;
} fs_in;

struct Material
{
	vec3 tint;
	float specularStrength;
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
	vec3 norm = normalize(fs_in.normal);
	vec3 lightDir = normalize(lights[0].location - fs_in.worldLocation);
	vec3 viewDir = normalize(cameraLocation - fs_in.worldLocation);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
	vec3 specular = lights[0].color * material.specularStrength * spec;

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = lights[0].color * diff;

	float ambientStrength = 0.1;
	vec3 ambient = lights[0].color * ambientStrength;

	vec3 result = (ambient + diffuse + specular) * material.tint;
	fragColor = vec4(result, 1.0);
}