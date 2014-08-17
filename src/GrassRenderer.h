#pragma once

#include "glStd.h"
#include "GrassPatch.h"
#include "UniformBlocks.h"

class Texture;

class GrassRenderer
{
public:
	GrassRenderer();
	~GrassRenderer();

	void Initialize();

	void Update(float dt);

	void Render(const PerFrameBlock& perFrame, GLuint perObjectBufferRef, GLuint lastProgramReference);

private:
	GrassPatch* mpGrassPatch;

	GLuint mpGrassProgram;

	Texture* mpGrassTexture;

	GLuint mTimeUniformLocation;

	float mCurrentTime;
};