#version 430 core

#include <Shaders/uniforms_h.glsl>

in FragmentIn
{
	vec3 PositionW;
	vec3 Normal;
} fragIn;

out vec4 color;

void main()
{
	color = dot(fragIn.Normal, normalize(perFrame.directionalLight.Direction)) * vec4(1.0, 1.0, 1.0, 1.0);
	color += vec4(0.1);
	color.a = 1.0;
}