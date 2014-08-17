#include "GrassRenderer.h"
#include "TextureManager.h"
#include "LoadShaders.h"

GrassRenderer::GrassRenderer()
	:mpGrassPatch(NULL),
	mpGrassProgram(0),
	mpGrassTexture(NULL),
	mTimeUniformLocation(0),
	mCurrentTime(0.0f)
{
	mpGrassPatch = new GrassPatch(3200000, 4252522U);
}

GrassRenderer::~GrassRenderer()
{
	delete mpGrassPatch;
	mpGrassPatch = NULL;

	glDeleteProgram(mpGrassProgram);
}

void GrassRenderer::Initialize()
{
	//Generate grass patch
	mpGrassPatch->Initialize();

	//Load grass texture
	TextureManager& textureManager = TextureManager::getInstance();

	textureManager.loadTexture("grassblade.dds");
	mpGrassTexture = textureManager.getTexture("grassblade.dds");

	//Load grass shader
	ShaderInfo grassShaderInfo[] = 
	{
		{ GL_VERTEX_SHADER, "Shaders/grass_vs.glsl" },
		{ GL_GEOMETRY_SHADER, "Shaders/grass_gs.glsl" },
		{ GL_FRAGMENT_SHADER, "Shaders/grass_fs.glsl" },
		{ GL_NONE, NULL },
	};

	mpGrassProgram = LoadShaders(grassShaderInfo);

	GLuint perFrameUniformIndex = glGetUniformBlockIndex(mpGrassProgram, "UniformPerFrameBlock");
	glUniformBlockBinding(mpGrassProgram, perFrameUniformIndex, 1);

	GLuint perObjectUniformIndex = glGetUniformBlockIndex(mpGrassProgram, "UniformPerObjectBlock");
	glUniformBlockBinding(mpGrassProgram, perObjectUniformIndex, 2);

	mTimeUniformLocation = glGetUniformLocation(mpGrassProgram, "Time");
}

void GrassRenderer::Update(float dt)
{
	mCurrentTime += dt;
}

void GrassRenderer::Render(const PerFrameBlock& perFrame, GLuint perObjectBufferRef, GLuint lastProgramReference)
{
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(mpGrassProgram);

	glUniform1f(mTimeUniformLocation, mCurrentTime);

	glActiveTexture(GL_TEXTURE0);
	mpGrassTexture->bind();

	mpGrassPatch->Render();

	//glDisable(GL_BLEND);

	glUseProgram(lastProgramReference);
}