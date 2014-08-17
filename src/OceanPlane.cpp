#include "OceanPlane.h"
#include "LoadShaders.h"

using namespace std;

OceanPlane::OceanPlane(Camera* baseCamera)
	:mpVertexBuffer(0),
	mpIndexBuffer(0),
	mpRenderProgram(0),
	mpCamera(baseCamera)
{
	mCameraProj = baseCamera->getProj();
	mCameraView = baseCamera->getView();
}

OceanPlane::~OceanPlane()
{
	glDeleteVertexArrays(1, &mpVertexArray);
	glDeleteBuffers(1, &mpIndexBuffer);
	glDeleteBuffers(1, &mpVertexBuffer);

	glDeleteProgram(mpRenderProgram);
}

void OceanPlane::Initialize()
{
	ShaderInfo shaderInfo[] =
	{
		{ GL_VERTEX_SHADER, "Shaders/ocean_vs.glsl" },
		{ GL_FRAGMENT_SHADER, "Shaders/ocean_fs.glsl" },
		{ GL_NONE, NULL }
	};

	mpRenderProgram = LoadShaders(shaderInfo);

	glUseProgram(mpRenderProgram);

	GLuint perFrameUniformIndex = glGetUniformBlockIndex(mpRenderProgram, "UniformPerFrameBlock");
	glUniformBlockBinding(mpRenderProgram, perFrameUniformIndex, 1);

	GLuint perObjectUniformIndex = glGetUniformBlockIndex(mpRenderProgram, "UniformPerObjectBlock");
	glUniformBlockBinding(mpRenderProgram, perObjectUniformIndex, 2);

	GLuint oceanUniformIndex = glGetUniformBlockIndex(mpRenderProgram, "UniformOceanBlock");
	glUniformBlockBinding(mpRenderProgram, oceanUniformIndex, 3);

	//Load the plane
	vector<vec3> vertices;
	vector<GLuint> indices;

	generatePlane(400, 400, vertices, indices);

	mIndexCount = indices.size();

	glGenBuffers(1, &mpIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mpIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &mpVertexArray);
	glBindVertexArray(mpVertexArray);

	glGenBuffers(1, &mpVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mpVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void OceanPlane::Render(const PerFrameBlock& perFrame, GLuint perObjectBufferRef, GLuint lastProgramReference)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(mpRenderProgram);

	mat4 oceanWorld = mat4(1.0f);
	mat4 oceanMVP = perFrame.ViewProj * oceanWorld;

	PerObjectBlock objectBlock;
	objectBlock.World = oceanWorld;
	objectBlock.WorldViewProj = oceanMVP;
	objectBlock.WorldInvTranspose = glm::inverse(glm::transpose(oceanWorld));

	OceanUniformBlock oceanBlock;
	oceanBlock.projection = mpCamera->getProj();
	oceanBlock.inverseProj = glm::inverse(mpCamera->getProj());
	oceanBlock.inverseView = glm::inverse(perFrame.View);

	glBindBuffer(GL_UNIFORM_BUFFER, perObjectBufferRef);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PerObjectBlock), &objectBlock, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_UNIFORM_BUFFER, mpOceanUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(OceanUniformBlock), &oceanBlock, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, mpOceanUniformBuffer);


	glBindBuffer(GL_ARRAY_BUFFER, mpVertexBuffer);
	glBindVertexArray(mpVertexArray);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (char*)NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mpIndexBuffer);

	//glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);

	glUseProgram(lastProgramReference);
	glDisable(GL_BLEND);
}

void OceanPlane::generatePlane(int c, int r, vector<vec3>& vertices, vector<GLuint>& indices)
{
	vertices.clear();
	indices.clear();

	float halfWidth = 1.0f;
	float halfHeight = 1.0f;

	float dx = 2.0f / ((float)c - 1.0f);
	float dy = 2.0f / ((float)r - 1.0f);

	for (int x = 0; x < c; x++)
	{
		float currentX = -halfWidth + x * dx;

		for (int y = 0; y < r; y++)
		{
			vertices.push_back(vec3(currentX, -halfHeight + y * dy, 0.0f));
		}
	}

	for (unsigned x = 0; x < r; x++)
	{
		for (unsigned z = 0; z < c; z++)
		{
			indices.push_back(x + z * c);
			indices.push_back(x + (z + 1) * c);
			indices.push_back(x + 1 + (z + 1) * c);
			indices.push_back(x + z * c);
			indices.push_back(x + 1 + (z + 1) * c);
			indices.push_back(x + 1 + z * c);
		}
	}

	mat4 inverseProj = glm::inverse(mCameraProj);
	mat4 inverseView = glm::inverse(mCameraView);

	//for (int i = 0; i < vertices.size(); i++)
	//{
		//transformPoint(vertices[i], inverseProj, inverseView);
	//}
}

void OceanPlane::transformPoint(vec3& point, const mat4& inverseProjection, const mat4& inverseView)
{
	vec3 planetCenter = vec3(0.0f, -6378.1 * 1000.0, 0.0f);

	point = vec3(inverseProjection * vec4(point, 1.0f));
	point = vec3(inverseView * vec4(point, 1.0f));

	vec3 direction = glm::normalize(point - vec3(mpCamera->getPosition()));

	float t;

	bool intersected = intersectRaySphere(vec3(mpCamera->getPosition()), direction, planetCenter, 6378.1 * 1000.0, t, point);

	if (!intersected)
	{
		point = planetCenter;
	}
}

bool OceanPlane::intersectRaySphere(const vec3& origin, const vec3& direction, const vec3& sphereCenter, float sphereRadius, float& t, vec3& intersection)
{
	vec3 m = origin - sphereCenter;
	float b = glm::dot(m, direction);
	float c = glm::dot(m, m) - sphereRadius * sphereRadius;

	if(c > 0.0f && b > 0.0f) 
		return false;

	float discr = b * b - c;
	
	if (discr < 0.0f) 
		return false;

	t = -b - sqrtf(discr);

	if (t < 0.0f)
		t = 0.0f;

	intersection = origin + t * direction;

	return true;
}