#include "RenderableMesh.h"
#include "Geometry.h"
#include "Texture.h"

RenderableMesh::RenderableMesh()
	:vertexArray(0),
	indexBuffer(0),
	vertexBuffer(0),
	mpDiffuseTexture(NULL),
	mpBumpMap(NULL),
	mpSpecularMap(NULL),
	mpMaterial(NULL),
	mpEmissiveMap(NULL)
{

}

RenderableMesh::~RenderableMesh()
{
	glDeleteVertexArrays(1, &vertexArray);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &indexBuffer);

	delete mpMaterial;
}

void RenderableMesh::Initialize(const Mesh& mesh)
{
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.Indices.size() * sizeof(GLuint), &mesh.Indices[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh.Vertices.size(), &mesh.Vertices[0], GL_STATIC_DRAW);

	indexCount = mesh.Indices.size();

	glBindVertexArray(0);
}

void RenderableMesh::Initialize(const ColoredMesh& mesh)
{
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.Indices.size() * sizeof(GLuint), &mesh.Indices[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (char*)NULL);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (char*)NULL + 12);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(ColoredVertex), (char*)NULL + 24);

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ColoredVertex) * mesh.Vertices.size(), &mesh.Vertices[0], GL_STATIC_DRAW);

	indexCount = mesh.Indices.size();

	glBindVertexArray(0);
}

void RenderableMesh::Draw(const PerObjectBlock& parentBlock, GLuint bufferRef)
{
	PerObjectBlock objectBlock = parentBlock;

	if (mpDiffuseTexture)
	{
		objectBlock.hasDiffuseTexture = true;
		glActiveTexture(GL_TEXTURE0);
		mpDiffuseTexture->bind();
	}
	else if (!objectBlock.hasDiffuseTexture)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	if (mpBumpMap)
	{
		objectBlock.hasBumpMap = true;
		glActiveTexture(GL_TEXTURE1);
		mpBumpMap->bind();
	}
	else if (!objectBlock.hasBumpMap)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	if (mpSpecularMap)
	{
		objectBlock.hasSpecularMap = true;
		glActiveTexture(GL_TEXTURE2);
		mpSpecularMap->bind();
	}
	else if (!objectBlock.hasSpecularMap)
	{
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	if (mpEmissiveMap)
	{
		objectBlock.hasEmissiveMap = true;
		glActiveTexture(GL_TEXTURE3);
		mpEmissiveMap->bind();
	}
	else if (!objectBlock.hasEmissiveMap)
	{
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	if (mpMaterial)
		objectBlock.ObjectMaterial = *mpMaterial;

	//Set it back for other objects
	glActiveTexture(GL_TEXTURE0);

	glBindBuffer(GL_UNIFORM_BUFFER, bufferRef);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PerObjectBlock), &objectBlock, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	glBindVertexArray(vertexArray);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 12);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 24);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 32);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)NULL + 44);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
}