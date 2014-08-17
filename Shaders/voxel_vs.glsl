#version 430 core

#include <Shaders/uniforms_h.glsl>
#include <Shaders/utilities.glsl>

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
//layout (location = 2) in vec4 color;

out FragmentIn
{
	vec3 PositionW;
	vec3 Normal;
} fragIn;

void main()
{
	gl_Position = perObject.WorldViewProj * vec4(position, 1.0f);
	fragIn.PositionW = (perObject.World * vec4(position, 1.0f)).xyz;
	fragIn.Normal = mat3(perObject.WorldInvTranspose) * normal;
	
	logDepthFunction();
}