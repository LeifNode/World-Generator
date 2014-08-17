#include "HeightmapGenerator.h"
#include "LoadShaders.h"
#include "Texture.h"

HeightmapGenerator::HeightmapGenerator(int resolution)
	:mComputeProgram(0),
	mpUniformBuffer(0),
	mpGenerationTarget(NULL),
	mResolution(resolution)
{
}

HeightmapGenerator::~HeightmapGenerator()
{
	//glDeleteTextures(1, &mpIntermediateTexture);
	glDeleteBuffers(1, &mpUniformBuffer);
	glDeleteProgram(mComputeProgram);
	mComputeProgram = 0;
}

void HeightmapGenerator::Initialize()
{
	ShaderInfo computeInfo[] = 
	{
		{ GL_COMPUTE_SHADER, "Shaders/terrain_cs.glsl" },
		{ GL_NONE, NULL }
	};

	mComputeProgram = LoadShaders(computeInfo);

	glGenBuffers(1, &mpUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, mpUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(MeshGenerationBuffer), NULL, GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, mpUniformBuffer);

	/*glGenTextures(1, &mpIntermediateTexture);
	glBindTexture(GL_TEXTURE_2D, mpIntermediateTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, mResolution, mResolution, 0, GL_RED, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);*/
}

void HeightmapGenerator::generateHeightmap(const dvec2& offset, double scale, const vec3& surfaceNorm)
{
	glUseProgram(mComputeProgram);

	glBindBuffer(GL_UNIFORM_BUFFER, mpUniformBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, mpUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(MeshGenerationBuffer), NULL, GL_DYNAMIC_DRAW);

	MeshGenerationBuffer* generationProperties = (MeshGenerationBuffer*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(MeshGenerationBuffer), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	generationProperties->gridDimensions = glm::ivec2(mResolution, mResolution);
	generationProperties->offset = offset;
	generationProperties->scale = scale;
	generationProperties->pad = 5.0;
	generationProperties->normalTransform = glm::mat4(1.0f);
	generationProperties->pointTransform = glm::translate(glm::dmat4(1.0f), dvec3(offset.x, 1.0, offset.y)) * glm::scale(glm::dmat4(1.0), dvec3(scale, 1.0f, scale));

	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindImageTexture(0, mpGenerationTarget->mpTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	//glBindImageTexture(1, mpIntermediateTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);

	//Verify that we are using texture with a resolution divible by 16 since the shader will not have proper coverage otherwise
	if (mResolution < 16 || mResolution % 16 != 0)
	{
		std::cout << "Texture resolution is not divisible by 16 this may cause problems with the shaders\n";
		throw -1;
	}

	//Each local group in the shader is currently specified as 16x16 threads
	GLuint workGroupCount = (GLuint)(mResolution / 16);

	//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glDispatchCompute(workGroupCount, workGroupCount, 1);

	//Wait for operations to finish
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}