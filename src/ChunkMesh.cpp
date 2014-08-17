#include "ChunkMesh.h"

ChunkMesh::ChunkMesh()
	:pSourceChunk(NULL),
	mTriangleCount(0)
{
}

ChunkMesh::~ChunkMesh()
{
	glDeleteBuffers(1, &mpVertexBuffer);
}

void ChunkMesh::Initialize()
{
	glGenBuffers(1, &mpVertexBuffer);
}

void ChunkMesh::rebuildChunk()
{
	if (pSourceChunk)
	{
		std::vector<VoxelVertex> vertices;

		int width, height, depth;

		width = pSourceChunk->getWidth();
		height = pSourceChunk->getHeight();
		depth = pSourceChunk->getDepth();

		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int z = 0; z < depth; z++)
				{
					if (pSourceChunk->vertexArray[x][y][z].enabled)
					{
						bool topActive = true;
						bool bottomActive = true;
						bool frontActive = true;
						bool backActive = true;
						bool leftActive = true;
						bool rightActive = true;

						if (x > 0)
							leftActive = !pSourceChunk->vertexArray[x-1][y][z].enabled;
						if (x < width - 1)
							rightActive = !pSourceChunk->vertexArray[x+1][y][z].enabled;
						if (y > 0)
							bottomActive = !pSourceChunk->vertexArray[x][y-1][z].enabled;
						if (y < height - 1)
							topActive = !pSourceChunk->vertexArray[x][y+1][z].enabled;
						if (z > 0)
							backActive = !pSourceChunk->vertexArray[x][y][z-1].enabled;
						if (z < depth - 1)
							frontActive = !pSourceChunk->vertexArray[x][y][z+1].enabled;

						addVertices(glm::ivec3(x, y, z), topActive, bottomActive, frontActive, backActive, leftActive, rightActive, vertices);
					}
				}
			}
		}

		mTriangleCount = vertices.size();

		glBindBuffer(GL_ARRAY_BUFFER, mpVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VoxelVertex) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
	}
}

void ChunkMesh::Render()
{
	glBindBuffer(GL_ARRAY_BUFFER, mpVertexBuffer);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelVertex), (char*)NULL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelVertex), (char*)NULL + 12);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(VoxelVertex), (char*)NULL + 24);

	glDrawArrays(GL_TRIANGLES, 0, mTriangleCount);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void ChunkMesh::addVertices(const glm::ivec3& position, bool topActive, bool bottomActive, bool frontActive, bool backActive, bool leftActive, bool rightActive, std::vector<VoxelVertex>& vertOut)
{
	float hw = 0.5f;
	float hh = 0.5f;
	float hd = 0.5f;

	vec3 basePosition = vec3(position.x, position.y, position.z);

	if (topActive)
	{
		vertOut.push_back(VoxelVertex(basePosition + vec3(-hw, hh, -hd), vec3(0.0f, 1.0f, 0.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(-hw, hh, hd), vec3(0.0f, 1.0f, 0.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(hw, hh, -hd), vec3(0.0f, 1.0f, 0.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(-hw, hh, hd), vec3(0.0f, 1.0f, 0.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(hw, hh, hd), vec3(0.0f, 1.0f, 0.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(hw, hh, -hd), vec3(0.0f, 1.0f, 0.0f)));
	}

	if (bottomActive)
	{
		vertOut.push_back(VoxelVertex(basePosition + vec3(-hw, -hh, -hd), vec3(0.0f, -1.0f, 0.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(hw, -hh, -hd), vec3(0.0f, -1.0f, 0.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(-hw, -hh, hd), vec3(0.0f, -1.0f, 0.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(-hw, -hh, hd), vec3(0.0f, -1.0f, 0.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(hw, -hh, -hd), vec3(0.0f, -1.0f, 0.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(hw, -hh, hd), vec3(0.0f, -1.0f, 0.0f)));
	}

	if (frontActive)
	{
		vertOut.push_back(VoxelVertex(basePosition + vec3(-hw, hh, hd), vec3(0.0f, 0.0f, 1.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(hw, -hh, hd), vec3(0.0f, 0.0f, 1.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(hw, hh, hd), vec3(0.0f, 0.0f, 1.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(-hw, hh, hd), vec3(0.0f, 0.0f, 1.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(-hw, -hh, hd), vec3(0.0f, 0.0f, 1.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(hw, -hh, hd), vec3(0.0f, 0.0f, 1.0f)));
	}

	if (backActive)
	{
		vertOut.push_back(VoxelVertex(basePosition + vec3(-hw, hh, -hd), vec3(0.0f, 0.0f, -1.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(hw, hh, -hd), vec3(0.0f, 0.0f, -1.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(hw, -hh, -hd), vec3(0.0f, 0.0f, -1.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(-hw, hh, -hd), vec3(0.0f, 0.0f, -1.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(hw, -hh, -hd), vec3(0.0f, 0.0f, -1.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(-hw, -hh, -hd), vec3(0.0f, 0.0f, -1.0f)));
	}

	if (rightActive)
	{
		vertOut.push_back(VoxelVertex(basePosition + vec3(hw, hh, -hd), vec3(1.0f, 0.0f, 0.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(hw, -hh, hd), vec3(1.0f, 0.0f, 0.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(hw, -hh, -hd), vec3(1.0f, 0.0f, 0.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(hw, hh, -hd), vec3(1.0f, 0.0f, 0.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(hw, hh, hd), vec3(1.0f, 0.0f, 0.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(hw, -hh, hd), vec3(1.0f, 0.0f, 0.0f)));
	}

	if (leftActive)
	{
		vertOut.push_back(VoxelVertex(basePosition + vec3(-hw, hh, -hd), vec3(-1.0f, 0.0f, 0.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(-hw, -hh, -hd), vec3(-1.0f, 0.0f, 0.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(-hw, -hh, hd), vec3(-1.0f, 0.0f, 0.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(-hw, hh, -hd), vec3(-1.0f, 0.0f, 0.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(-hw, -hh, hd), vec3(-1.0f, 0.0f, 0.0f)));
		vertOut.push_back(VoxelVertex(basePosition + vec3(-hw, hh, hd), vec3(-1.0f, 0.0f, 0.0f)));
	}
}