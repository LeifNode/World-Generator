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