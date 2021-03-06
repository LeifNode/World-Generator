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

#include <Shaders/LightHelper.glsl>

layout(std140) uniform UniformPerObjectBlock
{
	mat4 World;
	mat4 WorldInvTranspose;
	mat4 WorldViewProj;
	mat4 TexTransform;
	Material ObjectMaterial;

	bool hasDiffuseTexture;
	bool hasBumpMap;
	bool hasSpecularMap;
	bool hasEmissiveMap;
} perObject;

layout(std140) uniform UniformPerFrameBlock
{
	mat4 ViewProj;
	mat4 View;
	mat4 Proj;
	mat4 ViewInvTrans;

	vec3 ViewPosition;
	float pad;

	float nearClippingPlane;
	float farClippingPlane;
	vec2 pad2;

	DirectionalLight directionalLight;
	PointLight pointLight;
	SpotLight spotLight;
	
	dvec3 ViewPositionD;
} perFrame;

layout(std140) uniform UniformParticleSystemBlock
{
	vec4 Color;
	float Radius;
} perParticleSystem;