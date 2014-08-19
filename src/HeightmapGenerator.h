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

#pragma once

#include "glStd.h"

class Texture;

class HeightmapGenerator
{
#pragma pack(push, 1)
	struct MeshGenerationBuffer
	{
		dvec2 offset;
		double scale;
		double pad;
		glm::mat4 normalTransform;
		glm::dmat4 pointTransform;
		glm::ivec2 gridDimensions;
	};
#pragma pack(pop)
public:
	HeightmapGenerator(int resolution);
	~HeightmapGenerator();

	void Initialize();

	void setGenerationTarget(Texture* target) { mpGenerationTarget = target; }
	void generateHeightmap(const dvec2& offset, double scale, const vec3& surfaceNorm);

private:
	int mResolution;

	GLuint mComputeProgram;
	GLuint mpUniformBuffer;

	GLuint mpIntermediateTexture;

	Texture* mpGenerationTarget;
};