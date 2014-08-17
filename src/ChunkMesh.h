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