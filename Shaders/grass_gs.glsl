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

#include <Shaders/grass_h.glsl>

layout (points) in;
layout (triangle_strip, max_vertices = 12) out;

out GS_Interface
{
	vec4 PositionW;
	vec2 TexCoord;
	float YellowingFactor;
	float DarknessFactor;
} gsOut;

void main()
{
	vec2 windVec = normalize(vec2(-1.0, -3.0));

	vec3 position = gl_in[0].gl_Position.xyz;
	position.xz *= 500.0;
	
	float distance = length(perFrame.ViewPosition - position);
	distance *= 0.04;
	
	int sampleCount = clamp(int(4.0 / distance), 2, 6);
	
	float colorVal = hash(float(gl_PrimitiveIDIn));
	
	vec3 offsetVec = vec3(hash(colorVal), 0.0, hash(colorVal * 3045325.0)); //Just random numbers
	offsetVec *= hash(offsetVec.x) - 0.5;
	
	offsetVec = normalize(offsetVec);
	vec3 bendVector = cross(offsetVec, vec3(0.0, 1.0, 0.0));
	float bendFactor = hash(bendVector.x + bendVector.y) + 0.2;
	float offsetNoise = -0.2 + fbm( Time * windVec + position.xy, 0.1, 2.0, 0.5, 5);
	
	for (int i = 0; i < sampleCount; i++)
	{
		float heightProgress = (i + 1) / float(sampleCount);
		float offsetFactor = offsetNoise * heightProgress;
	
		vec3 heightVec = vec3(0.0, heightProgress * 5.0, 0.0);
		
		//Calculate base blade positions where it is standing straight up
		vec3 worldPos1 = position + offsetVec * 0.12 + bendVector * heightProgress * bendFactor * 2.0;
		vec3 worldPos2 = position - offsetVec * 0.12 + bendVector * heightProgress * bendFactor * 2.0;
		worldPos1.y += heightVec.y;
		worldPos2.y += heightVec.y;
		
		//Determine passive bent position
		worldPos1.y -= 2.5 * bendFactor * heightProgress * heightProgress * - 1.0 * offsetFactor * 0.5; 
		worldPos2.y -= 2.5 * bendFactor * heightProgress * heightProgress * - 1.0 * offsetFactor * 0.5;
		
		//Add wind vector
		worldPos1.xz += windVec * offsetFactor;
		worldPos2.xz += windVec * offsetFactor;
		
		gl_Position = perFrame.ViewProj * vec4(worldPos1, 1.0);
		gsOut.PositionW = vec4(worldPos1, 1.0);
		gsOut.TexCoord = vec2(0.0, heightProgress);
		gsOut.YellowingFactor = colorVal;
		gsOut.DarknessFactor = heightProgress;
		
		logDepthFunction();
		
		EmitVertex();
		
		gl_Position = perFrame.ViewProj * vec4(worldPos2, 1.0);
		gsOut.PositionW = vec4(worldPos2, 1.0);
		gsOut.TexCoord = vec2(1.0, heightProgress);
		gsOut.YellowingFactor = colorVal;
		gsOut.DarknessFactor = heightProgress;
		
		logDepthFunction();
		
		EmitVertex();
	}
	
	EndPrimitive();
}