//Tessellation control shader

#version 430 core

#include <Shaders/uniforms_h.glsl>

layout (vertices = 4) out;

void main()
{
	gl_TessLevelInner[0] = 200.0;
    gl_TessLevelInner[1] = 200.0;
    gl_TessLevelOuter[0] = 200.0;
    gl_TessLevelOuter[1] = 200.0;
    gl_TessLevelOuter[2] = 200.0;
    gl_TessLevelOuter[3] = 200.0;

    gl_out[gl_InvocationID].gl_Position	= normalize(gl_in[gl_InvocationID].gl_Position);
	
	//gl_out[gl_InvocationID].gl_Position = gl_out[gl_InvocationID].gl_Position / 100.0;
}