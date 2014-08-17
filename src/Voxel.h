#pragma once

#include "glStd.h"

struct Voxel
{
	friend class VoxelChunk;

	bool enabled;
	glm::u8vec3 position;
	glm::u8vec4 color;

private:
	Voxel() { ZeroMemory(this, sizeof(this)); enabled = false; }
	~Voxel() {}
};