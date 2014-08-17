#version 430 core

#include <Shaders/uniforms_h.glsl>

layout (location = 0) in vec4 position;

void main()
{
	gl_Position = position;
}