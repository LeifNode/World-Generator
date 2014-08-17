#pragma once

#include "glStd.h"
#include "UniformBlocks.h"

class Texture;

struct SkyboxVert
{
	vec3 position;

	SkyboxVert() 
	{ ZeroMemory(this, sizeof(this)); }

	SkyboxVert(vec3 pos)
		:position(pos)
	{}
};

class SkyboxRenderer
{
public:
	SkyboxRenderer();
	~SkyboxRenderer();

	void Initialize();

	void Render(const PerFrameBlock& perFrame, GLuint perObjectBufferRef, GLuint lastProgramReference);

private:
	void generateSkybox(std::vector<SkyboxVert>& verticesOut, std::vector<GLuint>& indicesOut);

private:
	GLuint mpSkyboxRenderProgram;

	GLuint mVertexBuffer;
	GLuint mIndexBuffer;
	GLuint mVertexArray;

	int mIndexCount;

	Texture* mpSkyboxTexture;
};