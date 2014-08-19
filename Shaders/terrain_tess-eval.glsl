//The MIT License (MIT)
//
//Copyright (c) 2014 Leif Erkenbrach
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.

//Tessellation evaulation shader
//NOT USED

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