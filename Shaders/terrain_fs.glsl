#version 430 core

#include <Shaders/terrain_h.glsl>

layout (location = 0) uniform sampler2D heightMap;

in FragmentIn
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

out vec4 color;

const vec4 sandColor = vec4(0.882, 0.662, 0.372, 1.0);
//const vec4 forestColor = vec4(34.0 / 255.0, 139.0 / 255.0, 34.0 / 255.0, 1.0);
const vec4 forestColor = vec4(33.0 / 255.0, 65.0 / 255.0, 33.0 / 255.0, 1.0);
const vec4 rockColor = vec4(0.41, 0.38, 0.422, 1.0);
const vec4 blackColor = vec4(0.0, 0.0, 0.0, 1.0);
const vec4 oceanColor = vec4(0.2f, 0.3f, 0.6f, 1.0f);
const vec4 turqouseColor = vec4(0.2f, 0.8f, 0.7f, 1.0f);
const vec4 dirtColor = vec4(0.51, 0.4, 0.22, 1.0);

void main()
{
	fragIn.SphereNormal = normalize(fragIn.SphereNormal);
	//fragIn.Normal = fragIn.WorldPosSpherified;
	fragIn.Normal = texture(heightMap, fragIn.TexCoord).rgb * 2.0 - 1.0;

	float noiseValue = denormalizeTerrainHeight(texture(heightMap, fragIn.TexCoord).a);
	float dirtFactor = 1.0 - max(0.0, min((dot(fragIn.SphereNormal, fragIn.Normal) - 0.5) * 4.0, 1.0));
	
	color = mix(blackColor, oceanColor, smoothstep(-500.0, -30.0, noiseValue));
	color = mix(color, turqouseColor, smoothstep(-100.0, -10.0, noiseValue));
	color = mix(color, sandColor, smoothstep(-30.0, 5.0, noiseValue));
	color = mix(color, forestColor, smoothstep(0.0, 50.0, noiseValue));
	color = mix(color, rockColor, smoothstep(10000.0, 12000.0, noiseValue));
	
	//color = mix(color, dirtColor, dirtFactor);
	
	//color.xyz = texture(heightMap, fragIn.TexCoord).rgb;
	
	vec3 toEye = normalize(perFrame.ViewPosition - fragIn.WorldPosNoise);

	vec4 ambient = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	vec4 diffuse = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	vec4 specular = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	vec4 A, D, S;
	
	Material mat;
	mat.Ambient = vec4(1.0);
	mat.Diffuse = vec4(1.5);
	mat.Specular = vec4(1.0);
	mat.Specular.w = 100.0;
	mat.Emissive = vec4(0.0);
	
	DirectionalLight sun = perFrame.directionalLight;
	//sun.Diffuse = fragIn.SunLight;
	//sun.Ambient = fragIn.AtmosphereAmbient;
	sun.Diffuse = vec4(1.0);
	sun.Ambient = vec4(0.0);

	ComputeDirectionalLight(mat, sun, fragIn.Normal, toEye, A, D, S);
	ambient += A + vec4(0.01);
	diffuse += D;
	//specular += S;
	
	color = color * (ambient + diffuse) + specular + mat.Emissive;
	
	float fog = exp2(-0.000004 * 0.000004 * fragIn.ProjectedPos.z * fragIn.ProjectedPos.z);
	fog = clamp(fog, 0.0, 1.0);
	
	//color = mix(ambient, color, fragIn.FogFactor);
	color *= fragIn.SunLight;
	color += 1.0 * fragIn.AtmosphereAmbient;
	color.a = 1.0f;
	
	//color.rgb = texture(heightMap, fragIn.TexCoord).rgb;
	//color.rgb = fract(texture(heightMap, fragIn.TexCoord).aaa * 8000.0);
	//color = fragIn.AtmosphereAmbient + fragIn.SunLight;
}