#version 430 core

#include <Shaders/ocean_h.glsl>

in FragmentIn
{
	vec3 Normal;
	vec3 ViewSpaceNormal;
	vec3 WorldPos;
	vec3 ViewSpacePos;
} fragIn;

out vec4 color;

const vec4 oceanColor = vec4(0.2f, 0.3f, 0.6f, 1.0f);

void main()
{
	vec3 toEye = normalize(-fragIn.ViewSpacePos);
	float fresnel = dot(toEye, fragIn.ViewSpaceNormal);

	color = vec4(0.5, 0.5, 0.5, 1.0) * dot(fragIn.Normal, perFrame.directionalLight.Direction);
	
	vec4 ambient = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 diffuse = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 specular = vec4(0.0, 0.0, 0.0, 1.0);
	
	Material mat;
	mat.Diffuse = oceanColor;
	mat.Specular = vec4(1.0, 1.0, 1.0, 40.0);
	mat.Ambient = vec4(0.0, 0.0, 0.0, 1.0);
	
	ComputeDirectionalLight(mat, perFrame.directionalLight, fragIn.Normal, mat3(inverseView) * toEye, ambient, diffuse, specular);
	
	color = ambient + diffuse + specular;
	color.a = -fresnel + 1.2;
}