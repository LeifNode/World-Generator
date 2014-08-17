#pragma once

#include "glStd.h"
#include "Geometry.h"
#include "RenderableMesh.h"
#include "UniformBlocks.h"

class Texture;

class GameObjectPlaceHolder
{
public:
	GameObjectPlaceHolder(const vec3& position);
	~GameObjectPlaceHolder();

	void addMesh(const Mesh& mesh);

	void Draw(const PerObjectBlock& parentBlock, GLuint bufferRef);

public:
	vec3 mPosition;
	std::vector<RenderableMesh*> mMeshes;
	std::vector<GameObjectPlaceHolder*> mChildren;

	Texture* mpDiffuseTexture;
	Texture* mpBumpMap;
	Texture* mpSpecularMap;
	Texture* mpEmissiveMap;

	Material* mpMaterial;

	mat4 mTextureTransform;

private:
};