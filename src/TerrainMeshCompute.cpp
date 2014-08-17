#include "TerrainMeshCompute.h"
#include "LoadShaders.h"

using namespace std;

TerrainMeshCompute::TerrainMeshCompute(int resolution, float scale)
	:mResolution(resolution),
	mScale(scale),
	mpVertexBuffer(0),
	mpVertexArray(0),
	mpUniformBuffer(0)
{
	resetRenderParams();

	mpIndexBuffers = new GLuint[4];
}

TerrainMeshCompute::~TerrainMeshCompute()
{
	glDeleteVertexArrays(1, &mpVertexArray);
	glDeleteBuffers(1, &mpVertexBuffer);
	glDeleteBuffers(4, mpIndexBuffers);
	glDeleteBuffers(1, &mpUniformBuffer);

	delete [] mpIndexBuffers;
	mpIndexBuffers = NULL;
}

void TerrainMeshCompute::Initialize()
{
	vector<vec4> vertices;
	vector<vector<GLushort>> indices;

	for (int i = 0; i < 4; i++)
		indices.push_back(vector<GLushort>());

	generateMesh(mResolution, mScale, vertices, indices);

	mIndexCount = indices[0].size();

	glGenVertexArrays(1, &mpVertexArray);
	glBindVertexArray(mpVertexArray);

	glGenBuffers(1, &mpVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mpVertexBuffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	glGenBuffers(4, mpIndexBuffers);

	for (int i = 0; i < 4; i++)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mpIndexBuffers[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indices[i].size(), &indices[i][0], GL_STATIC_DRAW);
	}

	//Compute shader initialization
	ShaderInfo computeInfo[] = 
	{
		{ GL_COMPUTE_SHADER, "Shaders/terrain_cs.glsl" },
		{ GL_NONE, NULL }
	};

	mComputeProgram = LoadShaders(computeInfo);
	glUseProgram(mComputeProgram);

	glGenTextures(1, &mpVertexTexture);

	glBindTexture(GL_TEXTURE_BUFFER, mpVertexTexture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, mpVertexBuffer);

	//Uniform buffer init
	glGenBuffers(1, &mpUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, mpUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(MeshGenerationBuffer), NULL, GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, mpUniformBuffer);
}

void TerrainMeshCompute::Generate(const dvec2& offset, double scale, const vec3& surfaceNorm)
{
	glUseProgram(mComputeProgram);

	glBindBuffer(GL_UNIFORM_BUFFER, mpUniformBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, mpUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(MeshGenerationBuffer), NULL, GL_DYNAMIC_DRAW);

	MeshGenerationBuffer* generationProperties = (MeshGenerationBuffer*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(MeshGenerationBuffer), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	generationProperties->gridDimensions = glm::ivec2(mResolution, mResolution);
	generationProperties->offset = offset;
	generationProperties->scale = scale;
	generationProperties->normalTransform = glm::dmat3(1.0);

	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindImageTexture(0, mpVertexTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glDispatchCompute(4, 1, 1); //256 threads for a total of 1024 vertices so we are expecting a 32x32 grid

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void TerrainMeshCompute::Render()
{
	glBindBuffer(GL_ARRAY_BUFFER, mpVertexBuffer);
	glBindVertexArray(mpVertexArray);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vec4), (char*)NULL);

	if (renderUL)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mpIndexBuffers[0]);
		glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_SHORT, 0); 
	}

	if (renderUR)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mpIndexBuffers[1]);
		glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_SHORT, 0); 
	}

	if (renderBL)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mpIndexBuffers[2]);
		glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_SHORT, 0); 
	}

	if (renderBR)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mpIndexBuffers[3]);
		glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_SHORT, 0); 
	}

	glDisableVertexAttribArray(0);
}

void TerrainMeshCompute::generateMesh(int resolution, float scale, std::vector<vec4>& vertexOut, std::vector<std::vector<GLushort>>& indexOut)
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
			float texX = z / (float)(resolution + 1);
			float texY = x / (float)(resolution + 1);
			vertexOut.push_back(vec4(currentX, 0.5f * scale, -halfDepth + z * dz, 0.0f));
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