#version 430 core

#include <Shaders/uniforms_h.glsl>

uniform samplerCube cubeMap;

in FragmentIn
{
	vec3 texCoord;
} fragIn;

out vec4 color;

void main()
{
	color = texture(cubeMap, fragIn.texCoord);
	//color = vec4(1.0, 0.0, 0.0, 1.0);
}