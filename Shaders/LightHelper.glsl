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