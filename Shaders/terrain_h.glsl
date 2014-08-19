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

#include <Shaders/uniforms_h.glsl>
#include <Shaders/atmosphere_h.glsl>

float denormalizeTerrainHeight(float height)
{
	return (height) * 20000.0 - 10000.0;
}

vec3 projectOntoSphere(vec3 pos)
{
	vec3 posSqr = pos * pos;
	vec3 result;
	
	result.x = pos.x * sqrt(1.0 - (posSqr.y / 2.0) - (posSqr.z / 2.0) + ((posSqr.y * posSqr.z) / 3.0));
	result.y = pos.y * sqrt(1.0 - (posSqr.z / 2.0) - (posSqr.x / 2.0) + ((posSqr.z * posSqr.x) / 3.0));
	result.z = pos.z * sqrt(1.0 - (posSqr.x / 2.0) - (posSqr.y / 2.0) + ((posSqr.x * posSqr.y) / 3.0));
	
	return result;
}

dvec3 projectOntoSphere(dvec3 pos)
{
	dvec3 posSqr = pos * pos;
	dvec3 result;
	
	result.x = pos.x * sqrt(1.0 - (posSqr.y / 2.0) - (posSqr.z / 2.0) + ((posSqr.y * posSqr.z) / 3.0));
	result.y = pos.y * sqrt(1.0 - (posSqr.z / 2.0) - (posSqr.x / 2.0) + ((posSqr.z * posSqr.x) / 3.0));
	result.z = pos.z * sqrt(1.0 - (posSqr.x / 2.0) - (posSqr.y / 2.0) + ((posSqr.x * posSqr.y) / 3.0));
	
	return result;
}

const int nSamples = 3;
const float fSamples = 3.0;

void calculateGroundScattering(vec3 posW, out vec4 frontColor, out vec4 secondaryColor, out float fogFactor)
{
	vec3 origin = vec3(0.0, -fInnerRadius, 0.0);
	vec3 adjustedCameraPos = perFrame.ViewPosition - origin;
	float cameraHeight = length(adjustedCameraPos);
	float cameraHeight2 = cameraHeight * cameraHeight;

	// Get the ray from the camera to the vertex and its length (which is the far point of the ray passing through the atmosphere)
	vec3 v3Pos = posW - origin;
	vec3 v3Ray = v3Pos - adjustedCameraPos;
	float fFar = length(v3Ray);
	v3Ray /= fFar;

	vec3 v3Start;
	
	if (length(adjustedCameraPos) > fOuterRadius)
	{
		// Calculate the closest intersection of the ray with the outer atmosphere (which is the near point of the ray passing through the atmosphere)
		float B = 2.0 * dot(adjustedCameraPos, v3Ray);
		float C = cameraHeight2 - fOuterRadius2;
		float fDet = max(0.0, B*B - 4.0 * C);
		float fNear = 0.5 * (-B - sqrt(fDet));
		
		// Calculate the ray's starting position, then calculate its scattering offset
		v3Start = adjustedCameraPos + v3Ray * fNear;
		fFar -= fNear;
	}
	else
	{
		v3Start = adjustedCameraPos;
	}
	
	//Calcluate fog factor
	float distance = fFar / 5000.0;
	fogFactor = exp2( -0.016 * 0.016 * distance * distance * 1.442697);
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	
	
	//Calculate color
	float fDepth = exp((fInnerRadius - fOuterRadius) / fScaleDepth);
	float fCameraAngle = dot(-v3Ray, v3Pos) / length(v3Pos);
	float fLightAngle = dot(perFrame.directionalLight.Direction, v3Pos) / length(v3Pos);
	float fCameraScale = scale(fCameraAngle);
	float fLightScale = scale(fLightAngle);
	float fCameraOffset = fDepth*fCameraScale;
	float fTemp = (fLightScale + fCameraScale);

	// Initialize the scattering loop variables
	float fSampleLength = fFar / fSamples;
	float fScaledLength = fSampleLength * fScale;
	vec3 v3SampleRay = v3Ray * fSampleLength;
	vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5;

	// Now loop through the sample rays
	vec3 v3FrontColor = vec3(0.0, 0.0, 0.0);
	vec3 v3Attenuate;
	for(int i=0; i<nSamples; i++)
	{
		float fHeight = length(v3SamplePoint);
		float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fHeight));
		float fScatter = fDepth*fTemp - fCameraOffset;
		v3Attenuate = exp(-fScatter * (v3InvWavelength * fKr4PI + fKm4PI));
		v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
		v3SamplePoint += v3SampleRay;
	}

	frontColor.rgb = v3FrontColor * (v3InvWavelength * fKrESun + fKmESun);

	// Calculate the attenuation factor for the ground
	secondaryColor.rgb = v3Attenuate;
}

const float worldScale = 6378.1 * 1000.0;

layout(std140) uniform TerrainUniformBuffer
{
	mat4 terrainTransform;
	float scale;
	vec2 offset;
	float pad2;
	float pad3;
} terrainPatchInfo;