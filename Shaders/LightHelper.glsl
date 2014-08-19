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

#include "Shaders/uniforms_h.glsl"
//LightHelper

struct Material
{
	vec4 Ambient;
	vec4 Diffuse;
	vec4 Specular; //w as specular power
	vec4 Emissive;
};

struct DirectionalLight
{
	vec4 Ambient;
	vec4 Diffuse;

	vec3 Direction;
	float pad;
};

struct PointLight
{ 
	vec4 Ambient;
	vec4 Diffuse;

	vec3 Position;
	float Range;

	vec3 Att;
	float pad;
};

struct SpotLight
{
	vec4 Ambient;
	vec4 Diffuse;

	vec3 Position;
	float Range;

	vec3 Direction;
	float Spot;

	vec3 Att;
	float pad;
};

void ComputeDirectionalLight(Material mat, DirectionalLight L, vec3 normal, vec3 toEye,
							 out vec4 ambient, out vec4 diffuse, out vec4 specular)
{
	ambient = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	specular = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	ambient = mat.Ambient * L.Ambient;

	vec3 lightVector = normalize(L.Direction);

	float diffuseFactor = dot(lightVector, normal);

	if (diffuseFactor > 0.0)
	{
		vec3 halfVector = normalize(toEye + lightVector);
		float specularFactor = pow(max(dot(halfVector, normal), 0.0f), mat.Specular.w);

		diffuse = mat.Diffuse * L.Diffuse * diffuseFactor;
		specular = mat.Specular * L.Diffuse * specularFactor;
	}
}

void ComputePointLight(Material mat, PointLight L, vec3 pos, vec3 normal, vec3 toEye,
				   out vec4 ambient, out vec4 diffuse, out vec4 spec)
{
	ambient = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	vec3 lightVector = L.Position - pos;

	float distance = length(lightVector);

	if (distance > L.Range)
		return;

	lightVector /= distance;

	ambient = mat.Ambient * L.Ambient;

	float diffuseFactor = dot(lightVector, normal);

	if (diffuseFactor > 0.0)
	{
		vec3 halfVector = normalize(toEye + lightVector);
		float specularFactor = pow(max(dot(halfVector, normal), 0.0f), mat.Specular.w);

		diffuse = mat.Diffuse * L.Diffuse * diffuseFactor;
		spec = mat.Specular * L.Diffuse * specularFactor;
	}

	float att = 1.0f / dot(L.Att, vec3(1.0f, distance, distance*distance));

	diffuse *= att;
	spec    *= att;
	spec = max(spec, 0.0f);
}

void ComputeSpotLight(Material mat, SpotLight L, vec3 pos, vec3 normal, vec3 toEye,
				  out vec4 ambient, out vec4 diffuse, out vec4 spec)
{
	ambient = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	vec3 lightVector = L.Position - pos;

	float distance = length(L.Position - pos);

	if (distance > L.Range)
		return;

	lightVector /= distance;

	ambient = mat.Ambient * L.Ambient;

	float diffuseFactor = dot(lightVector, normal);

	if (diffuseFactor > 0.0)
	{
		vec3 halfVector = normalize(toEye + lightVector);
		float specularFactor = pow(max(dot(halfVector, normal), 0.0f), mat.Specular.w);

		diffuse = mat.Diffuse * L.Diffuse * diffuseFactor;
		spec = mat.Specular * L.Diffuse * specularFactor;
	}

	float spot = pow(max(dot(-lightVector, L.Direction), 0.0f), L.Spot);

	float att = spot / dot(L.Att, vec3(1.0f, distance, distance*distance));

	ambient *= spot;
	diffuse *= att;
	spec    *= att;
}