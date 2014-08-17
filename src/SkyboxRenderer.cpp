#include "SkyboxRenderer.h"
#include "TextureManager.h"
#include "LoadShaders.h"

using namespace std;

SkyboxRenderer::SkyboxRenderer()
	:mVertexBuffer(0),
	mIndexBuffer(0),
	mVertexArray(0),
	mIndexCount(0),
	mpSkyboxTexture(NULL),
	mpSkyboxRenderProgram(0)
{

}

SkyboxRenderer::~SkyboxRenderer()
{
	glDeleteVertexArrays(1, &mVertexArray);
	glDeleteBuffers(1, &mVertexBuffer);
	glDeleteBuffers(1, &mIndexBuffer);

	glDeleteProgram(mpSkyboxRenderProgram);
}

void SkyboxRenderer::Initialize()
{
	//Load texture
	TextureManager::getInstance().loadTexture("skybox.dds");
	mpSkyboxTexture = TextureManager::getInstance().getTexture("skybox.dds");

	//Load shader
	ShaderInfo shaderInfo[] = 
	{
		{ GL_VERTEX_SHADER, "Shaders/skybox_vs.glsl" },
		{ GL_FRAGMENT_SHADER, "Shaders/skybox_fs.glsl" },
		{ GL_NONE, NULL }
	};

	mpSkyboxRenderProgram = LoadShaders(shaderInfo);

	glUseProgram(mpSkyboxRenderProgram);

	GLuint perFrameUniformIndex = glGetUniformBlockIndex(mpSkyboxRenderProgram, "UniformPerFrameBlock");
	glUniformBlockBinding(mpSkyboxRenderProgram, perFrameUniformIndex, 1);

	GLuint perObjectUniformIndex = glGetUniformBlockIndex(mpSkyboxRenderProgram, "UniformPerObjectBlock");
	glUniformBlockBinding(mpSkyboxRenderProgram, perObjectUniformIndex, 2);

	glUniform1i(glGetUniformLocation(mpSkyboxRenderProgram, "cubeMap"), 0);

	//Generate geometry
	vector<SkyboxVert> vertices;
	vector<GLuint> indices;

	generateSkybox(vertices, indices);

	glGenBuffers(1, &mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &mVertexArray);
	glBindVertexArray(mVertexArray);

	glGenBuffers(1, &mVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SkyboxVert) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	mIndexCount = indices.size();

	glBindVertexArray(0);
}

void SkyboxRenderer::generateSkybox(vector<SkyboxVert>& verticesOut, vector<GLuint>& indicesOut)
{
	//front face
	verticesOut.push_back(SkyboxVert(vec3(-0.5f, 0.5f, 0.5f)));
	verticesOut.push_back(SkyboxVert(vec3(0.5f, 0.5f, 0.5f)));
	verticesOut.push_back(SkyboxVert(vec3(-0.5f, -0.5f, 0.5f)));
	verticesOut.push_back(SkyboxVert(vec3(0.5f, -0.5f, 0.5f)));

	//back face
	verticesOut.push_back(SkyboxVert(vec3(-0.5f, 0.5f, -0.5f)));
	verticesOut.push_back(SkyboxVert(vec3(0.5f, 0.5f, -0.5f)));
	verticesOut.push_back(SkyboxVert(vec3(-0.5f, -0.5f, -0.5f)));
	verticesOut.push_back(SkyboxVert(vec3(0.5f, -0.5f, -0.5f)));

	//front face
	indicesOut.push_back(0);
	indicesOut.push_back(1);
	indicesOut.push_back(3);
	indicesOut.push_back(0);
	indicesOut.push_back(3);
	indicesOut.push_back(2);

	//back face
	indicesOut.push_back(4);
	indicesOut.push_back(6);
	indicesOut.push_back(7);
	indicesOut.push_back(4);
	indicesOut.push_back(7);
	indicesOut.push_back(5);
	

	//top face
	indicesOut.push_back(0);
	indicesOut.push_back(4);
	indicesOut.push_back(5);
	indicesOut.push_back(0);
	indicesOut.push_back(5);
	indicesOut.push_back(1);

	//bottom face
	indicesOut.push_back(2);
	indicesOut.push_back(3);
	indicesOut.push_back(7);
	indicesOut.push_back(2);
	indicesOut.push_back(7);
	indicesOut.push_back(6);

	//left face
	indicesOut.push_back(4);
	indicesOut.push_back(2);
	indicesOut.push_back(6);
	indicesOut.push_back(4);
	indicesOut.push_back(0);
	indicesOut.push_back(2);

	//right face
	indicesOut.push_back(5);
	indicesOut.push_back(3);
	indicesOut.push_back(1);
	indicesOut.push_back(5);
	indicesOut.push_back(7);
	indicesOut.push_back(3);
}

void SkyboxRenderer::Render(const PerFrameBlock& perFrame, GLuint perObjectBufferRef, GLuint lastProgramReference)
{
	glDisable(GL_DEPTH_TEST);
	glDepthMask(false);
	glUseProgram(mpSkyboxRenderProgram);

	glActiveTexture(GL_TEXTURE0);
	mpSkyboxTexture->bindCube();

	mat4 skyWorld = mat4(1.0f) * glm::scale(mat4(1.0f), vec3(1000.0f, 1000.0f, 1000.0f));
	mat4 skyMVP = perFrame.ViewProj * skyWorld;
	skyMVP[3] = vec4(0.0f, 0.0f, 0.0f, 1.0f);

	PerObjectBlock objectBlock;
	objectBlock.World = skyWorld;
	objectBlock.WorldViewProj = skyMVP;
	objectBlock.WorldInvTranspose = glm::inverse(glm::transpose(skyWorld));

	glBindBuffer(GL_UNIFORM_BUFFER, perObjectBufferRef);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PerObjectBlock), &objectBlock, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);

	glBindVertexArray(mVertexArray);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SkyboxVert), (char*)NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);

	glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);

	glDepthMask(true);
	glEnable(GL_DEPTH_TEST);
	glUseProgram(lastProgramReference);
}