#pragma once

#include "glStd.h"

struct Material
{
	Material() { ZeroMemory(this, sizeof(this)); }

	vec4 Ambient;
	vec4 Diffuse;
	vec4 Specular;
	vec4 Emissive;
};

struct DirectionalLight
{
	DirectionalLight() { ZeroMemory(this, sizeof(this)); }

	vec4 Ambient;
	vec4 Diffuse;
	vec3 Direction;
	float pad;
};

struct PointLight
{
	PointLight() { ZeroMemory(this, sizeof(this)); }

	vec4 Ambient;
	vec4 Diffuse;

	vec3 Position;
	float Range;

	vec3 Att;
	float pad;
};

struct SpotLight
{
	SpotLight() { ZeroMemory(this, sizeof(this)); }

	vec4 Ambient;
	vec4 Diffuse;

	vec3 Position;
	float Range;

	vec3 Direction;
	float Spot;

	vec3 Att;
	float Pad;
};