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

#pragma once

#include "glStd.h"
#include "Lights.h"

struct PerObjectBlock
{
	mat4 World;
	mat4 WorldInvTranspose;
	mat4 WorldViewProj;
	mat4 TexTransform;
	Material ObjectMaterial;

	//Gotta align the bytes. bool and GLboolean in c++ are 1 byte... in GLSL booleans are 4 bytes.. I think..
	GLuint hasDiffuseTexture;
	GLuint hasBumpMap;
	GLuint hasSpecularMap;
	GLuint hasEmissiveMap;

	PerObjectBlock()
	{ 
		ZeroMemory(this, sizeof(this));
		TexTransform = mat4(1.0f);
		hasDiffuseTexture = false;
		hasBumpMap = false;
		hasSpecularMap = false;
		hasEmissiveMap = false;
	}
};

struct PerFrameBlock
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
};

struct PerParticleSystemBlock
{
	vec4 Color;
	float Radius;
};

struct PerAtmosphereBlock
{
	vec3 v3InvWavelength;	// 1 / pow(wavelength, 4) for the red, green, and blue channels
	//float fCameraHeight;	// The camera's current height
	//float fCameraHeight2;	// fCameraHeight^2
	float fOuterRadius;		// The outer (atmosphere) radius
	float fOuterRadius2;	// fOuterRadius^2
	float fInnerRadius;		// The inner (planetary) radius
	float fInnerRadius2;	// fInnerRadius^2
	float fKrESun;			// Kr * ESun
	float fKmESun;			// Km * ESun
	float fKr4PI;			// Kr * 4 * PI
	float fKm4PI;			// Km * 4 * PI
	float fScale;			// 1 / (fOuterRadius - fInnerRadius)
	float fScaleDepth;		// The scale depth (i.e. the altitude at which the atmosphere's average density is found)
	float fScaleOverScaleDepth;	// fScale / fScaleDepth
	float g;
	float g2;
};