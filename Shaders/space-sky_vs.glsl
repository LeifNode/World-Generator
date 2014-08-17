//Based on shaders demonstrated in the article at http://http.developer.nvidia.com/GPUGems2/gpugems2_chapter16.html
// originally written by Sean O'Neil
//Sky from space

#version 430 core

#include <Shaders/atmosphere_h.glsl>

const int nSamples = 3;
const float fSamples = 3.0;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out FragmentIn
{
	vec3 direction;
	vec4 color;
	vec4 secondaryColor;
} fragIn;

void main()
{
	vec3 origin = vec3(0.0, -fInnerRadius, 0.0);
	vec3 adjustedCameraPos = perFrame.ViewPosition - origin;
	float cameraHeight = length(adjustedCameraPos);
	float cameraHeight2 = cameraHeight * cameraHeight;
	
	// Get the ray from the camera to the vertex, and its length (which is the far point of the ray passing through the atmosphere)
	vec3 v3Pos = position;
	vec3 v3Ray = v3Pos - adjustedCameraPos;
	float fFar = length(v3Ray);
	v3Ray /= fFar;

	// Calculate the closest intersection of the ray with the outer atmosphere (which is the near point of the ray passing through the atmosphere)
	float B = 2.0 * dot(adjustedCameraPos, v3Ray);
	float C = cameraHeight2 - fOuterRadius2;
	float fDet = max(0.0, B*B - 4.0 * C);
	float fNear = 0.5 * (-B - sqrt(fDet));

	// Calculate the ray's starting position, then calculate its scattering offset
	vec3 v3Start = adjustedCameraPos + v3Ray * fNear;
	fFar -= fNear;
	float fStartAngle = dot(v3Ray, v3Start) / fOuterRadius;
	float fStartDepth = exp(-1.0 / fScaleDepth);
	float fStartOffset = fStartDepth*scale(fStartAngle);

	// Initialize the scattering loop variables
	//gl_FrontColor = vec4(0.0, 0.0, 0.0, 0.0);
	float fSampleLength = fFar / fSamples;
	float fScaledLength = fSampleLength * fScale;
	vec3 v3SampleRay = v3Ray * fSampleLength;
	vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5;

	// Now loop through the sample rays
	vec3 v3FrontColor = vec3(0.0, 0.0, 0.0);
	for(int i=0; i<nSamples; i++)
	{
		float fHeight = length(v3SamplePoint);
		float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fHeight));
		float fLightAngle = dot(perFrame.directionalLight.Direction, v3SamplePoint) / fHeight;
		float fCameraAngle = dot(v3Ray, v3SamplePoint) / fHeight;
		float fScatter = (fStartOffset + fDepth*(scale(fLightAngle) - scale(fCameraAngle)));
		vec3 v3Attenuate = exp(-fScatter * (v3InvWavelength * fKr4PI + fKm4PI));
		v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
		v3SamplePoint += v3SampleRay;
	}

	// Finally, scale the Mie and Rayleigh colors and set up the varying variables for the pixel shader
	fragIn.secondaryColor.rgb = v3FrontColor * fKmESun;
	fragIn.color.rgb = v3FrontColor * (v3InvWavelength * fKrESun);
	gl_Position = perObject.WorldViewProj * vec4(position, 1.0f);
	fragIn.direction = adjustedCameraPos - v3Pos;
	
	//Logarithmic depth buffering
	logDepthFunction(); //utilities.glsl sets logarithmic depth
}