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

#version 430 core

#include <Shaders/uniforms_h.glsl>

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out FragmentIn
{
	vec4 Position;
	vec3 PositionW;
	vec3 Normal;
	vec2 TexCoord;
	vec3 Tangent;
	vec3 Binormal;
	float LogZ;
} fragIn;

void main()
{
	fragIn.Position = perObject.WorldViewProj * vec4(position, 1.0f);
	gl_Position = fragIn.Position;

	fragIn.PositionW = (perObject.World * vec4(position, 1.0f)).xyz;

	fragIn.Normal = mat3(perObject.WorldInvTranspose) * normal;
	fragIn.TexCoord = (perObject.TexTransform * vec4(texcoord.x, 0.0f, texcoord.y, 1.0f)).xz;
	fragIn.Tangent = mat3(perObject.WorldInvTranspose) * tangent;
	fragIn.Binormal = mat3(perObject.WorldInvTranspose) * bitangent;

	//Logarithmic depth buffering	
	const float C = 1;
	float FC = 1.0/log(perFrame.farClippingPlane * C + 1); 
	
	gl_Position.z = 2.0*log(gl_Position.w/perFrame.nearClippingPlane)/log(perFrame.farClippingPlane/perFrame.nearClippingPlane) - 1; 
    gl_Position.z *= gl_Position.w;
}
