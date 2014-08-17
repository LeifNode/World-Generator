#version 430 core

#include <Shaders/atmosphere_h.glsl>

in FragmentIn
{
	vec3 direction;
	vec4 color;
	vec4 secondaryColor;
} fragIn;

out vec4 color;

void main()
{
	float fCos = dot(perFrame.directionalLight.Direction, fragIn.direction) / length(fragIn.direction);
	float fMiePhase = 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos*fCos) / pow(1.0 + g2 - 2.0*g*fCos, 1.5);
	color = fragIn.color + fMiePhase * fragIn.secondaryColor;
	color.a = color.b + 6.7 * color.b;
	
	//HDR
	color.xyz = 1.0 - exp(color.xyz * -0.8);
}