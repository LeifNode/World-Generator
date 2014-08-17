#include "ParticleSystem.h"
#include "TextureManager.h"
#include "LoadShaders.h"
#include <random>
#include <time.h>
#include "LeapControl.h"


struct AttractorBlock
{
	vec4 attractors[4];
	LeapControlGroup fingers;
};

ParticleSystem::ParticleSystem()
	:mpParticleTexture(NULL),
	mpParticleRenderProgram(0),
	mPerFrameBuffer(0),
	mParticleCount(0),
	mParticleBuffer(0),
	mPerParticleBuffer(0),
	mVertexArray(0),
	mComputeDtLocation(0),
	mParticleVelocityBuffer(0)
{
}

ParticleSystem::~ParticleSystem()
{
	glDeleteBuffers(1, &mParticleBuffer);
	glDeleteBuffers(1, &mParticleVelocityBuffer);
	glDeleteBuffers(1, &mAttractorBuffer);
	glDeleteVertexArrays(1, &mVertexArray);
	glDeleteProgram(mpComputeProgram);
	glDeleteProgram(mpParticleRenderProgram);
}

void ParticleSystem::Initialize()
{
	ShaderInfo shaderInfo[] = 
	{
		{ GL_VERTEX_SHADER, "Shaders/particle_vs.glsl" },
		{ GL_GEOMETRY_SHADER, "Shaders/particle_gs.glsl" },
		{ GL_FRAGMENT_SHADER, "Shaders/particle_fs.glsl" },
		{ GL_NONE, NULL}
	};

	mpParticleRenderProgram = LoadShaders(shaderInfo);
	glUseProgram(mpParticleRenderProgram);

	TextureManager& textureManager = TextureManager::getInstance();

	textureManager.loadTexture("particle.dds");
	mpParticleTexture = textureManager.getTexture("particle.dds");

	glGenBuffers(1, &mPerParticleBuffer);

	GLuint perFrameUniformIndex = glGetUniformBlockIndex(mpParticleRenderProgram, "UniformPerFrameBlock");
	glUniformBlockBinding(mpParticleRenderProgram, perFrameUniformIndex, 1);

	GLuint perParticleBlockIndex = glGetUniformBlockIndex(mpParticleRenderProgram, "UniformParticleSystemBlock");
	glUniformBlockBinding(mpParticleRenderProgram, perParticleBlockIndex, 3);

	PerParticleSystemBlock perParticle;
	perParticle.Color = vec4(1.0f, 0.8f, 0.8f, 0.15f);
	perParticle.Radius = 0.3f;

	glBindBuffer(GL_UNIFORM_BUFFER, mPerParticleBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PerParticleSystemBlock), &perParticle, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, mPerParticleBuffer);

	glUniform1i(glGetUniformLocation(mpParticleRenderProgram, "particleTex"), 0);


	//Compute shader
	ShaderInfo computeInfo[] = 
	{
		{ GL_COMPUTE_SHADER, "Shaders/particle_cs.glsl" },
		{ GL_NONE, NULL }
	};

	mpComputeProgram = LoadShaders(computeInfo);
	glUseProgram(mpComputeProgram);

	mComputeDtLocation = glGetUniformLocation(mpComputeProgram, "dt");
	
	//Set up perticles
	InitializeParticleBuffer();

	glGenBuffers(1, &mAttractorBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, mAttractorBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(AttractorBlock), NULL, GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, mAttractorBuffer);
}

void ParticleSystem::InitializeParticleBuffer()
{
	srand((unsigned int)time(NULL));

	glGenVertexArrays(1, &mVertexArray);
	glBindVertexArray(mVertexArray);

	glGenBuffers(1, &mParticleBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mParticleBuffer);
	glBufferData(GL_ARRAY_BUFFER, PARTICLE_COUNT * sizeof(vec4), NULL, GL_DYNAMIC_COPY);

	vec4* particlePositionArray = (vec4*)glMapBufferRange(GL_ARRAY_BUFFER, 0, PARTICLE_COUNT * sizeof(vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	for (int i = 0; i < PARTICLE_COUNT; i++)
	{
		particlePositionArray[i] = vec4((rand() / (float)RAND_MAX) * 50.0f, (rand() / (float)RAND_MAX) * 50.0f, (rand() / (float)RAND_MAX) * 50.0f, 1.0f);
	}
	
	glUnmapBuffer(GL_ARRAY_BUFFER);

	mParticleCount = PARTICLE_COUNT;

	glGenBuffers(1, &mParticleVelocityBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mParticleVelocityBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * PARTICLE_COUNT, NULL, GL_DYNAMIC_COPY);

	vec4* particleVelocityArray = (vec4*)glMapBufferRange(GL_ARRAY_BUFFER, 0, PARTICLE_COUNT * sizeof(vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	for (int i = 0; i < PARTICLE_COUNT; i++)
	{
		particleVelocityArray[i] = glm::normalize(vec4(rand() / (float)RAND_MAX - 0.5f, rand() / (float)RAND_MAX - 0.5f, rand() / (float)RAND_MAX - 0.5f, 0.0f)) *0.1f;
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	//generate texture buffer objects
	glGenTextures(1, &mParticlePositionTexBuffer);
	glGenTextures(1, &mParticleVelocityTexBuffer);

	glBindTexture(GL_TEXTURE_BUFFER, mParticlePositionTexBuffer);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, mParticleBuffer);

	glBindTexture(GL_TEXTURE_BUFFER, mParticleVelocityTexBuffer);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, mParticleVelocityBuffer);
}

void ParticleSystem::Update(float dt)
{
	glUseProgram(mpComputeProgram);

	glUniform1f(mComputeDtLocation, dt * 1.0f);

	glBindBuffer(GL_UNIFORM_BUFFER, mAttractorBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(AttractorBlock), NULL, GL_DYNAMIC_DRAW);

	AttractorBlock* attractors = (AttractorBlock*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(AttractorBlock), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	for (int i = 0; i < 4; i++)
	{
		attractors->attractors[i] = vec4(0.0f);
	}

	attractors->fingers = LeapControl::getInstance().getState();

	attractors->attractors[0] = vec4(25.0f, 50.0f, 60.0f, 2500.0f);
	attractors->attractors[1] = vec4(25.0f, 0.0f, 90.0f, 150000.0f);
	attractors->attractors[2] = vec4(25.0f, 50.0f, 90.0f, 150000.0f);
	attractors->attractors[3] = vec4(25.0f, 0.0f,  60.0f, 2500.0f);

	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindImageTexture(0, mParticleVelocityTexBuffer, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(1, mParticlePositionTexBuffer, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glDispatchCompute(PARTICLE_GROUP_COUNT, 1, 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void ParticleSystem::Draw()
{
	glUseProgram(mpParticleRenderProgram);

	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	/*PerParticleSystemBlock perParticle;
	perParticle.Color = vec4(1.0f, 0.8f, 0.8f, 0.05f);
	perParticle.Radius = 3.1f;
	
	glBindBuffer(GL_UNIFORM_BUFFER, mPerParticleBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PerParticleSystemBlock), &perParticle, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, mPerParticleBuffer);*/

	glBindBufferBase(GL_UNIFORM_BUFFER, 3, mPerParticleBuffer);

	glActiveTexture(GL_TEXTURE0);
	mpParticleTexture->bind();

	glBindVertexArray(mVertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, mParticleBuffer);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);

	glDrawArrays(GL_POINTS, 0, mParticleCount);

	glDisableVertexAttribArray(0);

	glDepthMask(GL_TRUE); 
	glDisable(GL_BLEND);
}