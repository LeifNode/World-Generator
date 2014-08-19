//The MIT License (MIT)
//
//Copyright (c) 2014 Leif Erkenbrach
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.

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