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

#version 430 core

#include <Shaders/uniforms_h.glsl>

#extension GL_ARB_conservative_depth : enable 

layout(depth_less) out float gl_FragDepth;

uniform sampler2D diffuseTex;
uniform sampler2D bumpMap;
uniform sampler2D specularMap;
uniform sampler2D emissiveMap;

in FragmentIn
{
	vec4 Position;
	vec3 PositionW;
	vec3 Normal;
	vec2 TexCoord;
	vec3 Tangent;
	vec3 Binormal;
	float LogZ;
} fragIn;

layout (location = 0) out vec4 color;

void main()
{

	//gl_FragDepth = log(logz)*CF; 

	fragIn.Normal = normalize(fragIn.Normal);
	
	if (perObject.hasBumpMap)
	{
		fragIn.Tangent = normalize(fragIn.Tangent);
		fragIn.Binormal = normalize(fragIn.Binormal);

		vec4 bumpSample = texture(bumpMap, fragIn.TexCoord);
		bumpSample = (bumpSample * 2.0f) - 1.0f;
	
		fragIn.Normal += bumpSample.x * fragIn.Tangent + -bumpSample.y * fragIn.Binormal;
		fragIn.Normal = normalize(fragIn.Normal);
	}

	vec3 toEye = normalize(perFrame.ViewPosition - fragIn.PositionW);

	vec4 ambient = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	vec4 diffuse = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	vec4 specular = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	vec4 A, D, S;

	ComputeDirectionalLight(perObject.ObjectMaterial, perFrame.directionalLight, fragIn.Normal, toEye, A, D, S);
	ambient += A;
	diffuse += D;
	specular += S;

	ComputePointLight(perObject.ObjectMaterial, perFrame.pointLight, fragIn.PositionW, fragIn.Normal, toEye, A, D, S);
	ambient += A;
	diffuse += D;
	specular += S;

	ComputeSpotLight(perObject.ObjectMaterial, perFrame.spotLight, fragIn.PositionW, fragIn.Normal, toEye, A, D, S);
	ambient += A;
	diffuse += D;
	specular += S;

	if (perObject.hasSpecularMap)
	{
		specular *= texture(specularMap, fragIn.TexCoord);
	}

	if (perObject.hasDiffuseTexture)
	{
		color = texture(diffuseTex, fragIn.TexCoord) * (ambient + diffuse) + specular + perObject.ObjectMaterial.Emissive;
		color.a = perObject.ObjectMaterial.Diffuse.a;
	}
	else
	{
		color = ambient + diffuse + specular + perObject.ObjectMaterial.Emissive;
		color.a = 1.0f;
	}

	if (perObject.hasEmissiveMap)
	{
		float alpha = color.a;
		color += texture(emissiveMap, fragIn.TexCoord);
		color.a = alpha;
	}
}
