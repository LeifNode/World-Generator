#pragma once

#include "glStd.h"
#include "VoxelChunk.h"
#include "ChunkMesh.h"
#include "UniformBlocks.h"

class VoxelRenderer
{
public:
	VoxelRenderer();
	~VoxelRenderer();
	
	void Initialize();

	void Render(const PerFrameBlock& perFrame, GLuint perObjectBufferRef, GLuint lastProgramReference);

private:
	GLuint mVoxelProgram;

	VoxelChunk* mpChunk1;
	ChunkMesh* mpMesh1;
};