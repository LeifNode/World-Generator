#include "VoxelRenderer.h"
#include "LoadShaders.h"
#include "VoxelTreeGenerator.h"

VoxelRenderer::VoxelRenderer()
	:mVoxelProgram(0)
{
	mpChunk1 = new VoxelChunk(128, 128, 128);
	mpMesh1 = new ChunkMesh();
}

VoxelRenderer::~VoxelRenderer()
{
	glDeleteProgram(mVoxelProgram);

	delete mpChunk1;
	delete mpMesh1;

	mpChunk1 = nullptr;
	mpMesh1 = nullptr;
}
	
void VoxelRenderer::Initialize()
{
	mpMesh1->Initialize();

	ShaderInfo voxelProgramInfo[] =
	{
		{ GL_VERTEX_SHADER, "Shaders/voxel_vs.glsl" },
		{ GL_FRAGMENT_SHADER, "Shaders/voxel_fs.glsl" },
		{ GL_NONE, NULL }
	};

	mVoxelProgram = LoadShaders(voxelProgramInfo);

	GLuint perFrameUniformIndex = glGetUniformBlockIndex(mVoxelProgram, "UniformPerFrameBlock");
	glUniformBlockBinding(mVoxelProgram, perFrameUniformIndex, 1);

	GLuint perObjectUniformIndex = glGetUniformBlockIndex(mVoxelProgram, "UniformPerObjectBlock");
	glUniformBlockBinding(mVoxelProgram, perObjectUniformIndex, 2);

	/*for (int i = 0; i < 30000; i++)
	{
		mpChunk1->vertexArray[rand() % mpChunk1->getWidth()][rand() % mpChunk1->getHeight()][rand() % mpChunk1->getDepth()].enabled = false;
	}*/

	VoxelTreeGenerator generator;

	generator.setTargetChunk(mpChunk1);
	generator.generateSegments();
	generator.rasterizeSegments();

	mpMesh1->setTargetChunk(mpChunk1);
	mpMesh1->rebuildChunk();
}

void VoxelRenderer::Render(const PerFrameBlock& perFrame, GLuint perObjectBufferRef, GLuint lastProgramReference)
{
	//mpMesh1->setTargetChunk(mpChunk1);
	//mpMesh1->rebuildChunk();

	glUseProgram(mVoxelProgram);

	PerObjectBlock perObject;
	perObject.World = mat4(1.0f);
	perObject.WorldInvTranspose = glm::inverse(glm::transpose(mat4(1.0f)));
	perObject.WorldViewProj = perFrame.ViewProj * perObject.World;

	glBindBuffer(GL_UNIFORM_BUFFER, perObjectBufferRef);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PerObjectBlock), &perObject, GL_DYNAMIC_DRAW);

	mpMesh1->Render();

	glUseProgram(lastProgramReference);
}