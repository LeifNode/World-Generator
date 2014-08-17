#version 430 core

#include <Shaders/uniforms_h.glsl>

layout (location = 0) in vec3 position;

out FragmentIn
{
	vec3 texCoord;
} fragIn;

void main()
{
	gl_Position = perObject.WorldViewProj * vec4(position, 1.0);
	fragIn.texCoord = position;
}