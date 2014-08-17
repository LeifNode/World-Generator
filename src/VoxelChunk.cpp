#include "VoxelChunk.h"

VoxelChunk::VoxelChunk(int width, int height, int depth)
	:mWidth(width),
	mHeight(height),
	mDepth(depth)
{
	allocateMemory();
}

VoxelChunk::~VoxelChunk()
{
	deallocateMemory();
}

void VoxelChunk::allocateMemory()
{
	vertexArray = new Voxel**[mWidth]();

	for (int x = 0; x < mWidth; x++)
	{
		vertexArray[x] = new Voxel*[mHeight]();

		for (int y = 0; y < mHeight; y++)
		{
			vertexArray[x][y] = new Voxel[mDepth]();
		}
	}
}

void VoxelChunk::deallocateMemory()
{
	for (int x = 0; x < mWidth; x++)
	{
		for (int y = 0; y < mHeight; y++)
		{
			delete [] vertexArray[x][y];
		}

		delete [] vertexArray[x];
	}

	delete [] vertexArray;
}