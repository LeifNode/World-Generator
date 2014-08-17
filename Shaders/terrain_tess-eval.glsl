//Tessellation evaulation shader

#version 430 core

#include <Shaders/uniforms_h.glsl>
#include <Shaders/utilities.glsl>
#include <Shaders/noise.glsl>

layout (quads, fractional_odd_spacing, ccw) in;

out FragmentIn
{
	vec3 WorldPos;
	vec2 TexCoord;
	float noiseVal;
} fragIn;

void main()
{
	#define p(i)  gl_in[i].gl_Position

	float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
	
	fragIn.TexCoord = gl_TessCoord.xy;

    vec4 pos = v*(u*p(0) + (1-u)*p(1)) + (1-v)*(u*p(3) + (1-u)*p(2));
	
	float noiseValue = fbm(pos.xyz, 2.0, 2.0, 0.5, 20);
	fragIn.noiseVal = noiseValue;
	
	fragIn.WorldPos = pos.xyz;
	pos.y += noiseValue * 0.1;
	
	gl_Position = perObject.WorldViewProj * pos;
	
	
	logDepthFunction();
}