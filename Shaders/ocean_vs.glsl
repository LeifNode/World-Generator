#version 430 core

bool intersectRayWorld(vec3 o, vec3 d, vec3 center, float r, out float t, out vec3 q);

#include <Shaders/ocean_h.glsl>
#include <Shaders/noise.glsl>

layout (location = 0) in vec3 position;

out FragmentIn
{
	vec3 Normal;
	vec3 ViewSpaceNormal;
	vec3 WorldPos;
	vec3 ViewSpacePos;
} fragIn;

void main()
{
	vec4 pos = vec4(position, 1.0);
	
	//mat4 transform = inverseView * inverseProj;
	
	pos = inverseProj * pos;
	vec3 direction = normalize(pos.xyz);
	
	vec3 worldCenterLocal = (perFrame.View * vec4(worldPosition, 1.0)).xyz;
	
	float t;
	vec3 intersectionPoint = vec3(0.0, 0.0, 0.0);
	
	bool hit = intersectRayWorld(vec3(0.0, 0.0, 0.0), direction, worldCenterLocal, worldScale, t, intersectionPoint);
	
	if (hit)
	{
		pos.xyz = intersectionPoint;
	}
	else
	{
		pos.xyz = worldCenterLocal;
	}
	
	fragIn.ViewSpaceNormal = normalize(pos.xyz - worldCenterLocal);
	fragIn.Normal = mat3(inverseView) * fragIn.ViewSpaceNormal;
	
	fragIn.WorldPos = (inverseView * pos).xyz;
	
	
	float noiseValue = fbm(fragIn.WorldPos.xyx, 0.01, 2.0, 0.5, 5);
	
	//pos = (perFrame.View * vec4(fragIn.WorldPos, 1.0));
	//pos.xyz += noiseValue * fragIn.ViewSpaceNormal * 40.0;
	
	fragIn.ViewSpacePos = pos.xyz;
	
	gl_Position = projection * pos;
	
	logDepthFunction();
}