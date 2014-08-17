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