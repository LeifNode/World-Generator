#version 430 core

#include <Shaders/uniforms_h.glsl>

uniform sampler2D particleTex;

in GS_Interface
{
	vec4 PositionW;
	vec2 TexCoord;
} gsIn;

layout (location = 0) out vec4 color;

void main()
{
	color = texture(particleTex, gsIn.TexCoord) * perParticleSystem.Color;
}