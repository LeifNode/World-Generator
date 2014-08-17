#pragma once

#include "glStd.h"

class GrassPatch
{
public:
	GrassPatch(int bladeCount, int seed = 0);
	~GrassPatch();

	void Initialize();

	void Render();

private:
	int mBladeCount;
	unsigned int mSeed;

	GLuint mpVertexBuffer;
	GLuint mpVertexArray;
};