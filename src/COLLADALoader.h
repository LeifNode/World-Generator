#pragma once

#include "glStd.h"
#include "Geometry.h"
#include "COLLADATypes.h"
#include <tinyxml2\tinyxml2.h>

class COLLADALoader
{
public:
	COLLADALoader(const char* filePath);
	~COLLADALoader();

	void clear();

	void loadDocument();

	void parse();

	const collada::Model* getModel(const std::string& id) const;
	const collada::Effect* getEffect(const std::string& id) const;
	collada::SceneNode* getRootNode() const { return mpRootNode; }

	std::map<std::string, collada::Model*>::const_iterator modelsBegin() const { return mModels.begin(); }
	std::map<std::string, collada::Model*>::const_iterator modelsEnd() const { return mModels.end(); }

private:
	void loadAssetParameters();
	void loadModels();
	void loadEffects();
	void loadMaterials();
	void loadImages();
	void loadSceneNodes();

	void loadNode(collada::SceneNode* parent, tinyxml2::XMLElement* nodeElement);

	void loadNewParamElements(tinyxml2::XMLElement* profileElement, std::map<std::string, collada::Surface>& surfaceMap, std::map<std::string, collada::Sampler>& samplerMap);

	void readMeshSourceElement(tinyxml2::XMLElement* element, collada::MeshSource* storage);

	void readFloatArrayElement(tinyxml2::XMLElement* element, std::vector<float>& storage);
	void readIntArrayElement(tinyxml2::XMLElement* element, std::vector<int>& storage, int expectedSize = 0);
	void readColor(tinyxml2::XMLElement* element, vec4& out);

	void associateAccessors(tinyxml2::XMLElement* meshElement, collada::MeshAccessor* sourceArray);
	
	void constructModelGeometry(tinyxml2::XMLElement* geometryElement);

	int getInputIndexStride(tinyxml2::XMLElement* primitiveElement);

	std::string getExtensionFromPath(const char* path);


	void generateTangents();

private:
	collada::UpAxis mUpAxis;
	std::string mUnitScaleName;
	float mScalingFactor; //Corresponds to asset/unit/meter in collada spec

	collada::SceneNode* mpRootNode;

	std::map<std::string, collada::Model*> mModels;

	std::map<std::string, collada::Material> mMaterials;
	std::map<std::string, collada::Effect*> mEffects;
	std::map<std::string, collada::Image> mImages;

	std::string mFilePath;
	tinyxml2::XMLDocument mSceneDocument;

	std::map<std::string, collada::MeshSource*> mMeshSources;
};