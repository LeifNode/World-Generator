#include "GameObjectPlaceholder.h"
#include "Texture.h"

GameObjectPlaceHolder::GameObjectPlaceHolder(const vec3& position)
	:mPosition(position),
	mpDiffuseTexture(NULL),
	mpBumpMap(NULL),
	mpSpecularMap(NULL),
	mpEmissiveMap(NULL),
	mpMaterial(NULL),
	mTextureTransform(1.0f)
{
}

GameObjectPlaceHolder::~GameObjectPlaceHolder()
{
	for (auto it = mMeshes.begin(); it != mMeshes.end(); ++it)
	{
		delete (*it);
	}

	for (auto it = mChildren.begin(); it != mChildren.end(); ++it)
	{
		delete (*it);
	}

	delete mpMaterial;
}

void GameObjectPlaceHolder::addMesh(const Mesh& mesh)
{
	RenderableMesh* newMesh = new RenderableMesh();

	mMeshes.push_back(newMesh);

	mMeshes[mMeshes.size() - 1]->Initialize(mesh);
}

void GameObjectPlaceHolder::Draw(const PerObjectBlock& parentBlock, GLuint bufferRef)
{
	PerObjectBlock modelBlock = parentBlock;

	if (mpDiffuseTexture)
	{
		modelBlock.hasDiffuseTexture = true;
		glActiveTexture(GL_TEXTURE0);
		mpDiffuseTexture->bind();
	}
	else
		modelBlock.hasDiffuseTexture = false;

	if (mpBumpMap)
	{
		modelBlock.hasBumpMap = true;
		glActiveTexture(GL_TEXTURE1);
		mpBumpMap->bind();
	}
	else
		modelBlock.hasBumpMap = false;

	if (mpSpecularMap)
	{
		modelBlock.hasSpecularMap = true;
		glActiveTexture(GL_TEXTURE2);
		mpSpecularMap->bind();
	}
	else
		modelBlock.hasSpecularMap = false;

	if (mpEmissiveMap)
	{
		modelBlock.hasEmissiveMap = true;
		glActiveTexture(GL_TEXTURE3);
		mpEmissiveMap->bind();
	}
	else if (!modelBlock.hasEmissiveMap)
	{
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	if (mpMaterial)
		modelBlock.ObjectMaterial = *mpMaterial;

	modelBlock.TexTransform = mTextureTransform;

	for (auto it = mMeshes.begin(); it != mMeshes.end(); ++it)
	{
		(*it)->Draw(modelBlock, bufferRef);
	}

	for (auto it = mChildren.begin(); it != mChildren.end(); ++it)
	{
		(*it)->Draw(modelBlock, bufferRef);
	}
}