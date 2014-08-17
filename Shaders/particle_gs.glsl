#version 430 core

#include <Shaders/uniforms_h.glsl>

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

out GS_Interface
{
	vec4 PositionW;
	vec2 TexCoord;
} gsOut;

void main()
{
	vec3 position = gl_in[0].gl_Position.xyz;
	
	vec3 yUp = vec3(0.0f, 1.0f, 0.0f);
	vec3 toEye = normalize(vec3(perFrame.ViewPosition - position));
	
	vec3 right = normalize(cross(yUp, toEye));
	vec3 up = normalize(cross(right, toEye));
	
	float radius = perParticleSystem.Radius;
	
	vec2[4] texCoords;
	texCoords[0] = vec2(0.0f, 1.0f);
	texCoords[1] = vec2(0.0f, 0.0f);
	texCoords[2] = vec2(1.0f, 1.0f);
	texCoords[3] = vec2(1.0f, 0.0f);
	
	vec4[4] positionList;
	
	positionList[0] = vec4(position - (radius * right) - (radius * up), 1.0f);
	positionList[1] = vec4(position - (radius * right) + (radius * up), 1.0f);
	positionList[2] = vec4(position + (radius * right) - (radius * up), 1.0f);
	positionList[3] = vec4(position + (radius * right) + (radius * up), 1.0f);
	
	for (int i = 0; i < 4; i++)
	{
		gl_Position = perFrame.ViewProj * positionList[i];
		gsOut.PositionW = positionList[i];
		gsOut.TexCoord = texCoords[i];

		//Logarithmic depth buffering
		gl_Position.z = 2.0*log(gl_Position.w/0.1)/log(300000000.0/0.1) - 1; 
		gl_Position.z *= gl_Position.w;
		
		EmitVertex();
	}
	
	EndPrimitive();
}