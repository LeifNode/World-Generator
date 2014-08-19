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
#include "VoxelChunk.h"

class ChunkMesh
{
	struct VoxelVertex
	{
		VoxelVertex(const vec3& pos, const vec3& norm)
			:position(pos),
			normal(norm)
		{}
		~VoxelVertex() { }

		vec3 position;
		vec3 normal;
		vec3 color;
	};

public:
	ChunkMesh();
	~ChunkMesh();

	void Initialize();

	void setTargetChunk(VoxelChunk* chunk) { pSourceChunk = chunk; }
	void rebuildChunk();

	void Render();

	vec3 getPosition() const { return mPosition; }
	void setPosition(const vec3& position) { mPosition = position; }

	vec3 getScale() const { return mScale; }
	void setScale(const vec3& scale) { mScale = scale; }

private:
	void addVertices(const glm::ivec3& position, 
					 bool topActive, 
					 bool bottomActive, 
					 bool frontActive, 
					 bool backActive, 
					 bool leftActive, 
					 bool rightActive,
					 std::vector<VoxelVertex>& vertOut);

private:
	VoxelChunk* pSourceChunk;

	GLuint mpVertexBuffer;

	vec3 mPosition;
	vec3 mScale;

	int mTriangleCount;
};