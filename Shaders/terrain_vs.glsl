#version 430 core

#include <Shaders/terrain_h.glsl>
#include <Shaders/utilities.glsl>

layout (location = 0) uniform sampler2D heightMap;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out FragmentIn
{
	vec3 Normal;
	vec3 WorldPos;
	vec3 WorldPosSpherified;
	vec3 WorldPosNoise;
	vec4 ProjectedPos;
	vec3 SphereNormal;
	vec2 TexCoord;
	float NoiseVal;
	float Height;
	
	vec4 AtmosphereAmbient;
	vec4 SunLight;
	float FogFactor;
} fragIn;

void main()
{	
	fragIn.TexCoord = texCoord;
	fragIn.NoiseVal = texture(heightMap, texCoord).a;

    dvec3 pos = (dmat4(terrainPatchInfo.terrainTransform) * dvec4(position, 1.0)).xyz;
	fragIn.WorldPos = vec3(pos);
	
	pos = projectOntoSphere(pos);

	fragIn.WorldPosSpherified = vec3(pos);
	
	fragIn.SphereNormal = vec3(normalize(pos));
	//fragIn.Height = max(denormalizeTerrainHeight(fragIn.NoiseVal), -1.0);
	fragIn.Height = denormalizeTerrainHeight(fragIn.NoiseVal);
	
	pos *= worldScale;
	pos.y -= worldScale;
	
	pos += (fragIn.SphereNormal * fragIn.Height);
	fragIn.WorldPosNoise = vec3(pos);
	pos -= (fragIn.SphereNormal * fragIn.Height);
	
	pos -= perFrame.ViewPositionD;
	pos = mat3(perFrame.View) * pos;
	vec3 viewNormal = mat3(perFrame.View) * fragIn.SphereNormal;
	pos += (viewNormal * fragIn.Height);
	
	calculateGroundScattering(fragIn.WorldPosNoise, fragIn.AtmosphereAmbient, fragIn.SunLight, fragIn.FogFactor);
	
	//fragIn.AtmosphereAmbient = vec4(1.0, 1.0, 1.0, 1.0);
	//fragIn.SunLight = vec4(1.0, 1.0, 1.0, 1.0);
	
	fragIn.ProjectedPos = gl_Position = perFrame.Proj * vec4(pos, 1.0);
	
	logDepthFunction();
}