#pragma once

#include "glStd.h"
#include "UniformBlocks.h"

class GameObjectPlaceHolder;

const float worldScale = 6378.1f * 1000.0f;
const float atmosphereScale = worldScale * 1.025f;

class AtmosphereRenderer
{
public:
	AtmosphereRenderer();
	~AtmosphereRenderer();

	void Initialize();

	void Render(const PerFrameBlock& perFrame, GLuint perObjectBufferRef, GLuint lastProgramReference);

private:
	void InitializeAtmosphereUniform();

private:
	GLuint mpSkyFromAtmosphereProgram;
	GLuint mpSkyFromSpaceProgram;

	GLuint mpAtmosphereUniformBuffer;

	GameObjectPlaceHolder* mpAtmosphereSphere;
	GameObjectPlaceHolder* mpWorldSphere;
};