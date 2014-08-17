#pragma once

#include "Voxel.h"

class VoxelChunk
{
public:
	VoxelChunk(int width, int height, int depth);
	~VoxelChunk();

	Voxel*** vertexArray;

	int getWidth() const { return mWidth; }
	int getHeight() const { return mHeight; }
	int getDepth() const { return mDepth; }
	int getSize() const { return mWidth * mHeight * mDepth; }

private:
	void allocateMemory();
	void deallocateMemory();

private:
	int mWidth, mHeight, mDepth;
};