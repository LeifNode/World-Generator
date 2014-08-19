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

#include "glStd.h"
#include "Camera.h"
#include "UniformBlocks.h"

class OceanPlane
{
	struct OceanUniformBlock
	{
		mat4 projection;
		mat4 inverseView;
		mat4 inverseProj;
	};

public:
	OceanPlane(Camera* baseCamera);
	~OceanPlane();

	void Initialize();
	void Render(const PerFrameBlock& perFrame, GLuint perObjectBufferRef, GLuint lastProgramReference);

private:
	void generatePlane(int c, int r, std::vector<vec3>& vertices, std::vector<GLuint>& indices);
	void transformPoint(vec3& point, const mat4& inverseProjection, const mat4& inverseView);
	bool intersectRaySphere(const vec3& origin, const vec3& direction, const vec3& sphereCenter, float sphereRadius, float& t, vec3& intersection);

private:
	GLuint mpRenderProgram;

	GLuint mpVertexBuffer;
	GLuint mpVertexArray;
	GLuint mpIndexBuffer;
	GLuint mpOceanUniformBuffer;

	int mIndexCount;

	GLuint mCameraProjUniformLocation;

	mat4 mCameraProj;
	mat4 mCameraView;

	Camera* mpCamera;
};