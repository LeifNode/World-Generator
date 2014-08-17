#pragma once

#include "glStd.h"
#include "UniformBlocks.h"

struct Mesh;
struct ColoredMesh;
class Texture;

class RenderableMesh
{
public:
	RenderableMesh();
	~RenderableMesh();

	void Initialize(const Mesh& mesh);
	void Initialize(const ColoredMesh& mesh);

	void Draw(const PerObjectBlock& parentBlock, GLuint bufferRef);

	Texture* mpDiffuseTexture;
	Texture* mpBumpMap;
	Texture* mpSpecularMap;
	Texture* mpEmissiveMap;

	Material* mpMaterial;

private:
	GLuint vertexBuffer;
	GLuint indexBuffer;
	GLuint vertexArray;
	int indexCount;
};