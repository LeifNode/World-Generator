//The MIT License (MIT)
//
//Copyright (c) 2014 Leif Erkenbrach
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.

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