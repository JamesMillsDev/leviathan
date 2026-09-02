#version 460

layout(location = 0) out vec3 gLocation;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec3 gTangent;
layout(location = 3) out vec3 gBitangent;
layout(location = 4) out vec4 gAlbedoSpec;

in VS_OUT
{
    vec3 worldLocation;
    vec3 fragNormal;
    vec3 fragTangent;
    vec3 fragBitangent;
	vec2 uv0;
} fs_in;

struct Material
{
	vec3 tint;
	float specularStrength;
	sampler2D baseColor;
	sampler2D normalMap;
	sampler2D ormMap;
};

uniform Material material;

void main()
{
    gLocation = fs_in.worldLocation;

    vec3 N = normalize(fs_in.fragNormal);
    vec3 T = normalize(fs_in.fragTangent);
    vec3 B = normalize(fs_in.fragBitangent);
    mat3 TBN = mat3(T, B, N);

    vec3 normSample = texture(material.normalMap, fs_in.uv0).rgb * 2.0 - 1.0;
    gNormal = normalize(TBN * normSample);

    gTangent = fs_in.fragTangent;
    gBitangent = fs_in.fragBitangent;

    gAlbedoSpec.rgb = texture(material.baseColor, fs_in.uv0).rgb * material.tint;
    gAlbedoSpec.a = material.specularStrength;
}