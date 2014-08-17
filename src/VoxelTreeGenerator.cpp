#include "VoxelTreeGenerator.h"

VoxelTreeGenerator::VoxelTreeGenerator()
{

}

VoxelTreeGenerator::~VoxelTreeGenerator()
{

}

void VoxelTreeGenerator::generateSegments()
{
	int width = mpTargetChunk->getWidth();
	int height = mpTargetChunk->getWidth();
	int depth = mpTargetChunk->getDepth();

	vec3 startPoint = vec3(width / 2.0f, 0.0f, depth / 2.0f);
	vec3 direction = glm::normalize(vec3(0.1f, 1.0f, 0.1f));

	float length = height / 5.0f;

	generateSegments(startPoint, direction, length, 0);
}

void VoxelTreeGenerator::generateSegments(const vec3& startPoint, const vec3& direction, float length, int itCount)
{
	if (itCount < 7)
	{
		LineSegment segment;
		segment.start = startPoint;
		segment.end = startPoint + direction * length;

		mSegments.push_back(segment);

		float nextLength = length / 1.2f;

		vec3 xRotationVec = glm::normalize(glm::cross(direction, vec3(1.0f, 0.0f, 0.0f)));
		vec3 yRotationVec = glm::normalize(glm::cross(direction, vec3(0.0f, 1.0f, 0.0f)));
		vec3 zRotationVec = glm::normalize(glm::cross(direction, vec3(0.0f, 0.0f, 1.0f)));

		mat4 identity = mat4(1.0f);

		for (int i = 0; i < 6; i++)
		{
			vec3 newDirection = direction;
		
			float randX = ((rand() / (float)RAND_MAX) - 0.5f) * 10.0f * (5 - itCount);
			float randY = ((rand() / (float)RAND_MAX) - 0.5f) * 10.0f * (5 - itCount);
			float randZ = ((rand() / (float)RAND_MAX) - 0.5f) * 10.0f * (5 - itCount);

			mat4 rotationMatrix(1.0f);

			rotationMatrix = glm::rotate(rotationMatrix, randX, xRotationVec);
			rotationMatrix = glm::rotate(rotationMatrix, randY, yRotationVec);
			rotationMatrix = glm::rotate(rotationMatrix, randZ, zRotationVec); 

			newDirection = (mat3)rotationMatrix * newDirection;

			newDirection = glm::normalize(newDirection);

			generateSegments(segment.end, newDirection, nextLength, itCount + 1);
		}
	}
}

void VoxelTreeGenerator::rasterizeSegments()
{
	for (auto it = mSegments.begin(); it != mSegments.end(); ++it)
	{
		rasterizeSegment((*it));
	}
}

void VoxelTreeGenerator::rasterizePoint(const vec3& point)
{
	if (point.x > 0.0f && point.x < mpTargetChunk->getWidth() - 1.0f && 
		point.y > 0.0f && point.y < mpTargetChunk->getHeight() - 1.0f &&
		point.z > 0.0f && point.z < mpTargetChunk->getDepth() - 1.0f)
	{
		mpTargetChunk->vertexArray[(int)point.x][(int)point.y][(int)point.z].enabled = true;
	}
}

void VoxelTreeGenerator::rasterizeSegment(const LineSegment& segment)
{
	if (mpTargetChunk)
	{
		vec3 direction = segment.end - segment.start;
		float length = glm::length(direction);
		direction = glm::normalize(direction);

		int sampleCount = (int)length;
		float stepSize = length / sampleCount;

		for (int i = 0; i < sampleCount; i++)
		{
			vec3 point = segment.start + direction * (stepSize * i);
			rasterizePoint(point);
		}
	}
}