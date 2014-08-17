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