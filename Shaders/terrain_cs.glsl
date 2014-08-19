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

///Uses double precision for most operations which slows down generation of terrain by ~8x
/// Include noise.glsl, switch the dvec's to vec's, and doubles to floats in this file to use single floating point precision
/// Double precision is used to avoid artifacts in the generated normals when close to the ground

#version 430 core

//#include <Shaders/noise.glsl>
#include <Shaders/dnoise.glsl>
#include <Shaders/terrain_h.glsl>

#define GROUP_SIZE 16

layout (local_size_x = GROUP_SIZE, local_size_y = GROUP_SIZE) in;

layout (rgba32f, binding = 0) uniform image2D terrain_buffer;
//layout (r32f, binding = 1) uniform image2D intermediate_height_buffer;

shared float workGroupHeight[GROUP_SIZE][GROUP_SIZE];

layout(std140) uniform GenerationUniform
{
	dvec2 tileOffset;
	double tileScale;
	double pad;
	mat4 normalTransform;
	dmat4 pointTransform;
	ivec2 gridDimensions;
};

double calculateTerrainNoise(dvec3 point)
{
	//Fractional brownian motion for some simple terrain cell noise will probably be added later
	//return double(fbm(vec3(point), 15.0, 2.3, 0.45, 10) + 0.5 + 0.00005 * fbm(vec3(point), 900000.0, 2.7, 0.45, 7));
	
	double finalNoise;
	
	//Fractional brownian motion for some simple terrain cell noise will probably be added later
	finalNoise += fbm(dvec3(point), 15.0, 2.3, 0.45, 12) + 0.5; //Base terrain
	double mountainHillPlaneNoise = fbm(dvec3(point), 15.0, 2.3, 0.45, 2) * 1.3;//Mountains
	
	double mountainNoise = ridgedFbm(dvec3(point), 10.0, 2.2, 0.8, 3) * 0.00005;
	mountainNoise += ridgedFbm(dvec3(point), 70.0, 2.2, 0.8, 4) * mountainNoise + 0.005;
	//mountainNoise += ridgedFbm(dvec3(point), 3000.0, 2.2, 0.8, 3) * mountainNoise;
	//finalNoise += mountainNoise * max(0.0, mountainHillPlaneNoise);
	
	if (tileScale < pow(0.5, 6))
		finalNoise += (-ridgedFbm(dvec3(point), 4000.0, 2.8, 0.4, 9)) * 0.004;
	//finalNoise += 0.00005 * fbm(vec3(point), 900000.0, 2.7, 0.45, 7); //Dirt?
	//finalNoise *= cellularFbm(dvec3(point), 10.0, 2.3, 0.42, 5) + 0.5;
	//return double(fbm(vec3(point), 30.0, 2.0, 0.5, 3) + 0.5) - double(fbm(vec3(point), 600, 2.0, 0.5, 1));
	
	//Small rock formations
	double rockAreaNoise = clamp((cellularFbm(dvec3(point), 100000.0, 2.3, 0.45, 5) - 0.4)* 0.0009, 0.0, 1.0);
	
	return finalNoise + rockAreaNoise;
}

float normalizeTerrainHeight(double height)
{
	return float((height + 10000) / 20000);
}

void main()
{
	dvec3 meshPosition = dvec2(gl_GlobalInvocationID.x / double(gridDimensions.x - 1), gl_GlobalInvocationID.y / double(gridDimensions.y - 1)).xxy;
	dvec3 meshSpherePosition;
	meshPosition.y = 1.0;
	
	meshPosition.xz *= 2.0;
	meshPosition.xz -= 1.0;
	
	meshPosition.xz = meshPosition.zx;
	
	meshPosition.xz *= tileScale;
	meshPosition.xz += tileOffset;
	
	meshSpherePosition = projectOntoSphere(meshPosition);
	
	double noiseValue = calculateTerrainNoise(meshSpherePosition);
	
	workGroupHeight[gl_LocalInvocationID.x][gl_LocalInvocationID.y] = float(noiseValue);
	
	//Wait for all height values to get calculated
	barrier();
	
	//Calculate normals
	#define SAMPLE_OFFSET 1
	vec4 finalColor = vec4(0.0, 0.0, 0.0, noiseValue);
	
	int sampleX = int(gl_LocalInvocationID.x + SAMPLE_OFFSET);
	int sampleY = int(gl_LocalInvocationID.y + SAMPLE_OFFSET);
	double xOffset = SAMPLE_OFFSET;
	double yOffset = SAMPLE_OFFSET;
	float flipValue = 1.0; //So that normal does not get reversed on border pixels
	
	if (sampleX > gridDimensions.x / (GROUP_SIZE * 2) - 1)
	{
		sampleX = int(gl_LocalInvocationID.x) - SAMPLE_OFFSET;
		xOffset *= -1.0;
		flipValue *= -1.0;
	}
	if (sampleY > gridDimensions.y / (GROUP_SIZE * 2) - 1)
	{
		sampleY = int(gl_LocalInvocationID.y) - SAMPLE_OFFSET;
		yOffset *= -1.0;
		flipValue *= -1.0;
	} 
	
	//Calculate heights of points
	double baseHeight = denormalizeTerrainHeight(float(noiseValue));
	double dxHeight = denormalizeTerrainHeight(workGroupHeight[sampleX][gl_LocalInvocationID.y]);
	double dyHeight = denormalizeTerrainHeight(workGroupHeight[gl_LocalInvocationID.x][sampleY]);
	
	//Switched the x/yOffset here as a hack to get the normals aligned correctly...
	dvec3 dxNormal = projectOntoSphere(dvec3(meshPosition.x + (yOffset / double(gridDimensions.y - 1)) * tileScale, meshPosition.y, meshPosition.z));
	dvec3 dyNormal = projectOntoSphere(dvec3(meshPosition.x, meshPosition.y, meshPosition.z + (xOffset / double(gridDimensions.x - 1)) * tileScale));
	
	//Switched dx/yHeight here too.
	//Raise point in normal direction to get actual height
	dvec3 basePosition = meshSpherePosition * double(worldScale) + double(baseHeight) * meshSpherePosition;
	
	dvec3 dxPosition = dxNormal * worldScale + dyHeight * dxNormal;
	dvec3 dyPosition = dyNormal * worldScale + dxHeight * dyNormal;
	dvec3 dxVector = normalize(dxPosition - basePosition);
	dvec3 dyVector = normalize(dyPosition - basePosition);
	
	//Calculate normal and map to 0..1 space
	finalColor.rgb = (vec3(normalize(cross(dyVector, dxVector))) * flipValue) + 1.0;
	finalColor.rgb /= 2.0;
	
	//Store the final normal and height maps
	imageStore(terrain_buffer, ivec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y), finalColor);
}