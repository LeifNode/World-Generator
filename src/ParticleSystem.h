#pragma once

//Compute shader based on OpenGL programming guide 8th ed. implementation

#include "glStd.h"
#include "UniformBlocks.h"

class Texture;

struct Particle
{
	vec3 position;
	vec3 velocity;
};

class ParticleSystem
{
	static const int PARTICLE_GROUP_COUNT = 10000;
	static const int PARTICLE_GROUP_SIZE = 512;
	static const int PARTICLE_COUNT = PARTICLE_GROUP_COUNT * PARTICLE_GROUP_SIZE;

public:
	ParticleSystem();
	~ParticleSystem();

	void Initialize();

	void Update(float dt);
	void Draw();

private:
	void InitializeParticleBuffer();

private:
	Texture* mpParticleTexture;
	GLuint mpParticleRenderProgram;
	GLuint mpComputeProgram;

	GLuint mPerFrameBuffer;
	GLuint mPerParticleBuffer;
	GLuint mAttractorBuffer;

	GLuint mParticleBuffer;
	GLuint mParticleVelocityBuffer;
	GLuint mVertexArray;

	GLuint mParticlePositionTexBuffer;
	GLuint mParticleVelocityTexBuffer;

	int mParticleCount;

	GLuint mComputeDtLocation;
};