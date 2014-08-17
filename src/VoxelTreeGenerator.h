#pragma once

#include "glStd.h"
#include "VoxelChunk.h"

class VoxelTreeGenerator
{
	struct LineSegment
	{
		vec3 start;
		vec3 end;
	};

public:
	VoxelTreeGenerator();
	~VoxelTreeGenerator();

	void setTargetChunk(VoxelChunk* chunk) { mpTargetChunk = chunk; }
	void generateSegments();
	void generateSegments(const vec3& startPoint, const vec3& direction, float length, int itCount);
	void rasterizeSegments();

private:
	void rasterizePoint(const vec3& point);
	void rasterizeSegment(const LineSegment& segment);

private:
	VoxelChunk* mpTargetChunk;

	std::vector<LineSegment> mSegments;
};