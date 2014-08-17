#include "GrassPatch.h"

GrassPatch::GrassPatch(int bladeCount, int seed)
	:mBladeCount(bladeCount),
	mSeed(seed)
{
}

GrassPatch::~GrassPatch()
{
	glDeleteVertexArrays(1, &mpVertexArray);
	glDeleteBuffers(1, &mpVertexBuffer);
}

void GrassPatch::Initialize()
{
	vec3* vecArr = new vec3[mBladeCount]();

	srand(mSeed);

	for (int i = 0; i < mBladeCount; i++)
	{
		vecArr[i] = vec3(rand() / (float)RAND_MAX, 0.0f, rand() / (float)RAND_MAX);
		vecArr[i] = (vecArr[i] * 2.0f) - 1.0f;
	}

	glGenVertexArrays(1, &mpVertexArray);
	glBindVertexArray(mpVertexArray);

	glGenBuffers(1, &mpVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mpVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * mBladeCount, vecArr, GL_STATIC_COPY);

	glBindVertexArray(0);

	delete [] vecArr;

	std::cout << "Grass patch generated with " << mBladeCount << " total blades of grass\n";
}

void GrassPatch::Render()
{
	glBindBuffer(GL_ARRAY_BUFFER, mpVertexBuffer);
	glBindVertexArray(mpVertexArray);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (char*)NULL);

	glDrawArrays(GL_POINTS, 0, mBladeCount);

	glDisableVertexAttribArray(0);
}