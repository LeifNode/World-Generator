#pragma once

#include "glStd.h"

namespace collada
{
	struct MeshSource
	{
		std::vector<float> data;
		int stride;

		MeshSource()
		{ stride = 0; }
	};

	struct MeshAccessor
	{
		MeshSource* source;
		int offset;

		MeshAccessor()
		{ source = NULL; offset = 0; }
	};

	enum PrimitiveType
	{
		Triangle,
		PolyList,
	};

	enum UpAxis
	{
		Y_UP,
		Z_UP,
	};

	struct SubMesh
	{
		std::string effectId;
		Mesh mesh;
	};

	struct Model
	{
		std::vector<SubMesh*> subMeshes;

		Model()
		{}
		~Model()
		{
			for (auto it = subMeshes.begin(); it != subMeshes.end(); ++it)
				delete (*it);
		}
	};

	struct SceneNode
	{
		Model* model;
		std::string effectId;
		std::vector<SceneNode*> children;

		SceneNode()
		{ model = NULL; }

		~SceneNode()
		{
			for (auto it = children.begin(); it != children.end(); ++it)
				delete (*it);
		}
	};

	struct Effect
	{
		vec4 emission;
		vec4 ambient;
		vec4 diffuse;
		vec4 specular;
		float specularPower;

		std::string diffuseTextureId;
		std::string bumpMapId;
		std::string specularMapId;
		std::string emissiveMapId;

		Effect()
		{ 
			emission = vec4(0.0f);
			ambient = vec4(1.0f);
			diffuse = vec4(1.0f);
			specular = vec4(1.0f);
			specularPower = 1.0f;
		}
	};

	struct Material
	{
		std::string effectId;
	};

	struct Image
	{
		std::string filePath;

		Image() 
		{}
		Image(const std::string& path)
			:filePath(path)
		{}
	};
	
	struct Surface
	{
		std::string imageId;
	};

	struct Sampler
	{
		std::string surfaceId;
	};
};