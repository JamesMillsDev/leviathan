#version 460

struct Material
{
	vec3 tint;
};

uniform Material material;

out vec4 fragColor;

void main()
{
	fragColor = vec4(material.tint, 1.0);
}