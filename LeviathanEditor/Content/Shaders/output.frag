#version 460 

in vec2 uv0;

out vec4 fragColor;

uniform sampler2D finalRenderBuffer;

void main()
{
	fragColor = texture(finalRenderBuffer, uv0);
}