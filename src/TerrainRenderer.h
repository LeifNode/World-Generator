#pragma once

#include "glStd.h"

#include "UniformBlocks.h"
#include "HeightmapManager.h"

class TerrainMesh;
class TerrainLODQuadTree;
class GrassRenderer;
class Camera;

class TerrainRenderer
{
	struct TerrainUniformBuffer
	{
		mat4 terrainTransform;
		float scale;
		vec2 offset;
		float pad2;
		float pad3;
	};

public:
	TerrainRenderer(float planetRadius);
	~TerrainRenderer();

	void Initialize();

	void Update(const Camera& camera, float dt);
	void Render(const PerFrameBlock& perFrame, GLuint perObjectBufferRef, GLuint lastProgramReference);

private:
	void generateBox(std::vector<vec3>& vertices);

private:
	GLuint mpTerrainProgram;

	TerrainMesh* mpMesh;

	TerrainLODQuadTree* mpQuadTree;

	HeightmapManager* mpHeightmapManager;

	GLuint mpTerrainUniformBuffer;
	GLuint mpAtmosphereUniformBuffer;
	GLuint mTerrainDimensionLocation;

	float mPlanetRadius;

	GrassRenderer* mpGrassRenderer;
};