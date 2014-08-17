#pragma once

#include "glStd.h"

class TerrainMeshCompute
{
	struct MeshGenerationBuffer
	{
		dvec2 offset;
		double scale;
		double pad;
		glm::dmat3 normalTransform;
		glm::ivec2 gridDimensions;
		int test;
	};

public:
	TerrainMeshCompute(int resolution, float scale);
	~TerrainMeshCompute();

	void Initialize();

	void Generate(const dvec2& offset, double scale, const vec3& surfaceNorm);

	void Render();

	void resetRenderParams() { renderUL = renderUR = renderBL = renderBR = true; }

public:
	bool renderUL, renderUR, renderBL, renderBR;

private:
	void generateMesh(int resolution, float scale, std::vector<vec4>& vertexOut, std::vector<std::vector<GLushort>>& indexOut);

private:
	int mResolution;
	float mScale;

	int mIndexCount;

	GLuint mComputeProgram;

	GLuint mpVertexBuffer;
	GLuint mpVertexArray;
	GLuint* mpIndexBuffers;

	GLuint mpUniformBuffer;

	GLuint mpVertexTexture;
};