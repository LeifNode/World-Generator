#include "TerrainMesh.h"
#include "Texture.h"

using namespace std;

TerrainMesh::TerrainMesh(int resolution, float scale)
	:mResolution(resolution),
	mScale(scale),
	mIndexCount(0),
	mpCurrentTexture(NULL)
{
	resetRenderParams();
}

TerrainMesh::~TerrainMesh()
{
	glDeleteVertexArrays(1, &mpVertexArray);
	glDeleteBuffers(1, &mpVertexBuffer);
	glDeleteBuffers(4, mpIndexBuffers);

	delete [] mpIndexBuffers;
}

void TerrainMesh::Initialize()
{
	mpIndexBuffers = new GLuint[4];
	vector<TerrainVertex> vertices;
	vector<vector<GLuint>> indices;

	for (int i = 0; i < 4; i++)
		indices.push_back(vector<GLuint>());

	generateMesh(mResolution, mScale, vertices, indices);

	mIndexCount = indices[0].size();

	glGenVertexArrays(1, &mpVertexArray);
	glBindVertexArray(mpVertexArray);

	glGenBuffers(1, &mpVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mpVertexBuffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(TerrainVertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	glGenBuffers(4, mpIndexBuffers);

	for (int i = 0; i < 4; i++)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mpIndexBuffers[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices[i].size(), &indices[i][0], GL_STATIC_DRAW);
	}
}

void TerrainMesh::Render()
{
	if (mpCurrentTexture)
	{
		glActiveTexture(GL_TEXTURE0);
		mpCurrentTexture->bind();
	}

	glBindBuffer(GL_ARRAY_BUFFER, mpVertexBuffer);
	glBindVertexArray(mpVertexArray);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (char*)NULL);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (char*)NULL + 12);

	if (renderUL)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mpIndexBuffers[0]);
		glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0); 
	}

	if (renderUR)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mpIndexBuffers[1]);
		glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0); 
	}

	if (renderBL)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mpIndexBuffers[2]);
		glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0); 
	}

	if (renderBR)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mpIndexBuffers[3]);
		glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0); 
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void TerrainMesh::generateMesh(int resolution, float scale, vector<TerrainVertex>& vertexOut, vector<vector<GLuint>>& indexOut)
{
	vertexOut.clear();
	resolution++;

	vertexOut.reserve((resolution + 1) * (resolution + 1));

	float halfWidth = scale / 2.0f;
	float halfDepth = scale / 2.0f;

	float dx = scale / ((float)resolution - 1.0f);
	float dz = scale / ((float)resolution - 1.0f);

	for (int x = 0; x < resolution; x++)
	{
		float currentX = -halfWidth + x * dx;

		for (int z = 0; z < resolution; z++)
		{
			float texX = z / (float)(resolution - 1);
			float texY = x / (float)(resolution - 1);

			vertexOut.push_back(TerrainVertex(vec3(currentX, 0.5f * scale, -halfDepth + z * dz), vec2(texX, texY)));
		}
	}

	int halfResolution = resolution / 2;

	//Upper Left
	for (int x = 0; x < halfResolution; x++)
	{
		for (int z = 0; z < halfResolution; z++)
		{
			indexOut[0].push_back(x + z * resolution);
			indexOut[0].push_back(x + 1 + z * resolution);
			indexOut[0].push_back(x + 1 + (z + 1) * resolution);

			indexOut[0].push_back(x + z * resolution);
			indexOut[0].push_back(x + 1 + (z + 1) * resolution);
			indexOut[0].push_back(x + (z + 1) * resolution);
		}
	}

	//Upper Right
	for (int x = halfResolution; x < resolution - 1; x++)
	{
		for (int z = 0; z < halfResolution; z++)
		{
			indexOut[1].push_back(x + z * resolution);
			indexOut[1].push_back(x + 1 + z * resolution);
			indexOut[1].push_back(x + 1 + (z + 1) * resolution);

			indexOut[1].push_back(x + z * resolution);
			indexOut[1].push_back(x + 1 + (z + 1) * resolution);
			indexOut[1].push_back(x + (z + 1) * resolution);
		}
	}

	//Bottom left
	for (int x = 0; x < halfResolution; x++)
	{
		for (int z = halfResolution; z < resolution - 1; z++)
		{
			indexOut[2].push_back(x + z * resolution);
			indexOut[2].push_back(x + 1 + z * resolution);
			indexOut[2].push_back(x + 1 + (z + 1) * resolution);

			indexOut[2].push_back(x + z * resolution);
			indexOut[2].push_back(x + 1 + (z + 1) * resolution);
			indexOut[2].push_back(x + (z + 1) * resolution);
		}
	}

	//Bottom right
	for (int x = halfResolution; x < resolution - 1; x++)
	{
		for (int z = halfResolution; z < resolution - 1; z++)
		{
			indexOut[3].push_back(x + z * resolution);
			indexOut[3].push_back(x + 1 + z * resolution);
			indexOut[3].push_back(x + 1 + (z + 1) * resolution);

			indexOut[3].push_back(x + z * resolution);
			indexOut[3].push_back(x + 1 + (z + 1) * resolution);
			indexOut[3].push_back(x + (z + 1) * resolution);
		}
	}
}