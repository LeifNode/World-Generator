#pragma once

#include "glStd.h"

class Texture;

struct TerrainVertex
{
	vec3 position;
	vec2 texCoord;

	TerrainVertex()
	{ ZeroMemory(this, sizeof(this)); }

	TerrainVertex(const vec3& pos, const vec2& tex)
		:position(pos),
		texCoord(tex)
	{}
};

class TerrainMesh
{
public:
	TerrainMesh(int resolution, float scale);
	~TerrainMesh();

	void Initialize();

	void Render();

	bool renderUL, renderUR, renderBL, renderBR;

	void resetRenderParams() { renderUL = renderUR = renderBL = renderBR = true; }

	void setCurrentTexture(Texture* texture) { mpCurrentTexture = texture; }

	int getResolution() const { return mResolution; }

private:
	void generateMesh(int resolution, float scale, std::vector<TerrainVertex>& vertexOut, std::vector<std::vector<GLuint>>& indexOut);

private:
	GLuint mpVertexArray;
	GLuint mpVertexBuffer;

	GLuint* mpIndexBuffers;

	Texture* mpCurrentTexture;

	int mResolution;
	float mScale;
	int mIndexCount;
};