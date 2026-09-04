#version 460

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

uniform GBuffer gBuffer;
uniform vec3 cameraLocation;

out vec4 fragColor;

void main()
{
    vec4 baseColor = texture(gBuffer.tint, fs_in.uv0);
    fragColor = vec4(baseColor.rgb, 1.0);
}