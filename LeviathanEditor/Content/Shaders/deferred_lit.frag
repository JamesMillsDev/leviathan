#version 460 

struct GBuffer
{
    sampler2D location;
    sampler2D normal;
    sampler2D tangent;
    sampler2D biTangent;
    sampler2D albedoSpec;
    int debugPhase; 
};

uniform GBuffer gBuffer;

in vec2 fragUv0;

out vec4 fragColor;

void main()
{
    if(gBuffer.debugPhase == 1)
    {
        fragColor = vec4(texture(gBuffer.location, fragUv0).rgb, 1.0);
        return;
    }

    if(gBuffer.debugPhase == 2)
    {
        fragColor = vec4(texture(gBuffer.normal, fragUv0).rgb, 1.0);
        return;
    }

    if(gBuffer.debugPhase == 3)
    {
        fragColor = vec4(texture(gBuffer.tangent, fragUv0).rgb, 1.0);
        return;
    }

    if(gBuffer.debugPhase == 4)
    {
        fragColor = vec4(texture(gBuffer.biTangent, fragUv0).rgb, 1.0);
        return;
    }

    if(gBuffer.debugPhase == 5)
    {
        fragColor = vec4(texture(gBuffer.albedoSpec, fragUv0).rgb, 1.0);
        return;
    }

    if(gBuffer.debugPhase == 6)
    {
        fragColor = vec4(texture(gBuffer.albedoSpec, fragUv0).a, 1.0, 1.0, 1.0);
        return;
    }

    fragColor = vec4(1.0);
}