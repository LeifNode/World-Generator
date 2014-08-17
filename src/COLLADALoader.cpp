#include "COLLADALoader.h"
#include <stdio.h>

using namespace tinyxml2;
using namespace std;
using namespace collada;

COLLADALoader::COLLADALoader(const char* filePath)
	:mFilePath(filePath),
	mMeshSources(),
	mpRootNode(NULL)
{
}

COLLADALoader::~COLLADALoader()
{
	clear();
}

void COLLADALoader::clear()
{
	for (auto it = mModels.begin(); it != mModels.end(); ++it)
	{
		delete it->second;
	}

	mModels.clear();

	for (auto it = mEffects.begin(); it != mEffects.end(); ++it)
		delete it->second;

	mEffects.clear();

	mMaterials.clear();
	mImages.clear();
	mMeshSources.clear();

	delete mpRootNode;
	mpRootNode = NULL;
}

void COLLADALoader::loadDocument()
{
	FILE* modelFile;
	fopen_s( &modelFile, mFilePath.c_str(), "rb" );

	//Check if file loaded
	if (!modelFile)
	{
		cout << "Could not load file " << mFilePath << endl;
		throw -1;
	}

	mSceneDocument.LoadFile(modelFile);

	fclose(modelFile);
}

void COLLADALoader::loadAssetParameters()
{
	XMLElement* assetElement = mSceneDocument.FirstChildElement()->FirstChildElement("asset");
	assert(assetElement);

	XMLElement* unitElement = assetElement->FirstChildElement("unit");
	mUnitScaleName = unitElement->Attribute("name");
	mScalingFactor = unitElement->FloatAttribute("meter");

	const char* axisString = assetElement->FirstChildElement("up_axis")->GetText();

	if (strcmp(axisString, "Z_UP") == 0)
		mUpAxis = Z_UP;
	else if (strcmp(axisString, "Y_UP") == 0)
		mUpAxis = Y_UP;
	else
	{
		cout << "Unrecognized up axis" << endl;
		throw -1;
	}
}

void COLLADALoader::readMeshSourceElement(XMLElement* element, MeshSource* storage)
{
	XMLElement* floatArray = element->FirstChildElement("float_array");
	XMLElement* accessor = element->FirstChildElement("technique_common")->FirstChildElement("accessor");

	assert(floatArray != NULL);

	readFloatArrayElement(floatArray, storage->data);
	
	if (accessor != NULL)
	{
		storage->stride = accessor->IntAttribute("stride");
	}
	else
		std::cout << "No accessor found!" << std::endl;
}

void COLLADALoader::readFloatArrayElement(XMLElement* element, vector<float>& storage)
{
	assert(element->Attribute("count") != NULL);

	int count = element->IntAttribute("count");

	storage.clear();
	storage.reserve(count);

	const char* stringPtr = element->GetText();

	for (int i = 0; i < count; i++)
	{
		while (*stringPtr == ' ')
			stringPtr++;

		storage.push_back((float)atof(stringPtr));

		while (*stringPtr != ' ' && *stringPtr != '\0')
			stringPtr++;
	}
}

void COLLADALoader::readIntArrayElement(XMLElement* element, vector<int>& storage, int expectedSize)
{
	storage.clear();

	const char* stringPtr = element->GetText();

	if (expectedSize > 0)
		storage.reserve(expectedSize);

	while (*stringPtr != '\0')
	{
		while (*stringPtr == ' ' && *stringPtr != '\0')
			stringPtr++;

		if (*stringPtr == '\0')
			break;

		storage.push_back(atoi(stringPtr));

		while (*stringPtr != ' ' && *stringPtr != '\0')
			stringPtr++;
	}
}

void COLLADALoader::readColor(XMLElement* element, vec4& out)
{
	const char* stringPtr = element->GetText();

	int i = 0;

	while (*stringPtr != '\0' && i < 4)
	{
		while (*stringPtr == ' ' && *stringPtr != '\0')
			stringPtr++;

		out[i] = (float)atof(stringPtr);

		while (*stringPtr != ' ' && *stringPtr != '\0')
			stringPtr++;

		i++;
	}

	//If only a float3
	if (out.a == 0.0f && i < 4)
		out.a = 1.0f;
}

void COLLADALoader::associateAccessors(XMLElement* primitiveElement, MeshAccessor* sourceArray)
{
	XMLElement* vertexElement = primitiveElement->PreviousSiblingElement("vertices")->FirstChildElement("input");
	assert(vertexElement);

	sourceArray[0].source = mMeshSources[vertexElement->Attribute("source") + 1];
	assert(sourceArray[0].source);

	XMLElement* inputElement = primitiveElement->FirstChildElement("input");

	while (inputElement)
	{
		const char* semantic = inputElement->Attribute("semantic");
		const char* source = inputElement->Attribute("source") + 1;

		if (*(source - 1) != '#')
		{
			cout << "Invalid URL format. Expected #, Received URL: " << (source - 1) << endl;
			return;
		}

		int offset = inputElement->IntAttribute("offset");
		int arrayIndex = 0;

		if (strcmp(semantic, "NORMAL") == 0)
			arrayIndex = 1;
		else if (strcmp(semantic, "TEXCOORD") == 0)
			arrayIndex = 2;
		else if (strcmp(semantic, "TANGENT") == 0 || strcmp(semantic, "TEXTANGENT") == 0) //Not sure about the practical difference at the moment
			arrayIndex = 3;
		else if (strcmp(semantic, "BINORMAL") == 0 || strcmp(semantic, "TEXBINORMAL") == 0)
			arrayIndex = 4;


		if (arrayIndex != 0)
		{
			sourceArray[arrayIndex].source = mMeshSources[source];
			sourceArray[arrayIndex].offset = offset;
		}

		inputElement = inputElement->NextSiblingElement("input");
	}
}

void COLLADALoader::loadModels()
{
	XMLElement* geometryLibElement = mSceneDocument.FirstChildElement()->FirstChildElement("library_geometries");
	XMLElement* currentGeometryElement = NULL;

	if (geometryLibElement != NULL)
		currentGeometryElement = geometryLibElement->FirstChildElement("geometry");

	while (currentGeometryElement != NULL)
	{
		constructModelGeometry(currentGeometryElement);

		currentGeometryElement = currentGeometryElement->NextSiblingElement("geometry");
	}
}

void COLLADALoader::constructModelGeometry(XMLElement* geometryElement)
{
	if (mModels.find(geometryElement->Attribute("id")) != mModels.end() || 
		geometryElement->FirstChildElement("mesh") == NULL || 
		geometryElement->FirstChildElement("mesh")->FirstChildElement("source") == NULL)
	{
		/*if (destination != NULL)
			*destination = mModels[geometryElement->Attribute("id")];*/

		return;
	}

	Model* newModel = new Model();

	mModels[geometryElement->Attribute("id")] = newModel;

	/*if (destination != NULL)
		*destination = newMesh;*/

	XMLElement* meshElement = geometryElement->FirstChildElement("mesh");
	XMLElement* sourceElement = meshElement->FirstChildElement("source");
	XMLElement* primitiveElement = meshElement->FirstChildElement();

	MeshSource* vertexSource;

	//Read source elements for vertex parameters
	//Thr source elements just contain lists of floats most of the time
	while (sourceElement != NULL)
	{
		string sourceId(sourceElement->Attribute("id"));

		MeshSource* currentSource = new MeshSource();
		mMeshSources[sourceId] = currentSource;

		readMeshSourceElement(sourceElement, currentSource);

		sourceElement = sourceElement->NextSiblingElement("source");
	}

	//Getting position information for mesh
	string vertexSourceId(meshElement->FirstChildElement("vertices")->FirstChildElement("input")->Attribute("source") + 1);
	vertexSource = mMeshSources[vertexSourceId];

	//Since there can be a mix of multiple triangles and polylist elements that share the same sources
	while (primitiveElement != NULL)
	{
		PrimitiveType primitiveType;

		vector<int> polyListvCount;
		vector<int> pList;

		int vertexCount = 0;
		int primitiveCount = 0;

		int primitiveInputStride = 0;


		//Determine input primitive type and get its element
		if (strcmp(primitiveElement->Value(), "triangles") == 0)
		{
			primitiveType = Triangle;
		}
		else if (strcmp(primitiveElement->Value(), "polylist") == 0)
		{
			primitiveType = PolyList;
		}
		else if (strcmp(primitiveElement->Value(), "triangles") == 0)
		{
			cout << "Polygons are unsupported.\n";
			primitiveElement = primitiveElement->NextSiblingElement();
			continue;
		}
		else
		{
			primitiveElement = primitiveElement->NextSiblingElement();
			continue;
		}

		SubMesh* subMesh = new SubMesh();

		if (primitiveElement->Attribute("material") != NULL)
			subMesh->effectId = mMaterials[primitiveElement->Attribute("material")].effectId;

		primitiveCount = primitiveElement->IntAttribute("count");

		//Calculate final vertex count
		if (primitiveType == PolyList)
		{
			readIntArrayElement(primitiveElement->FirstChildElement("vcount"), polyListvCount, primitiveCount);

			for (unsigned i = 0; i < polyListvCount.size(); i++)
			{
				//if (i % 100 == 0)
					//cout << i << " " << polyListvCount[i] << " " << vertexCount << "\n";
				vertexCount += polyListvCount[i];
			}
		}
		else if (primitiveType == Triangle)
			vertexCount = primitiveCount * 3;

		//Clear meshes in case they already have data
		subMesh->mesh.Vertices.clear();
		subMesh->mesh.Indices.clear();
	
		//Avoid reallocs
		subMesh->mesh.Vertices.reserve(vertexCount);

		primitiveInputStride = getInputIndexStride(primitiveElement);

		MeshAccessor accessorArray[5]; //Corresponds to the source arrays for POSITION, NORMAL, TEXCOORD, TANGENT, and BINORMAL

		associateAccessors(primitiveElement, accessorArray);

		//Number of points on a face
		int pCount = 0;
	
		if (primitiveType == Triangle)
			pCount = 3;
		else
			pCount = polyListvCount[0];

		//Read the index list
		readIntArrayElement(primitiveElement->FirstChildElement("p"), pList, vertexCount);

		//Index that each face starts on
		int baseIndex = 0;

		Vertex vert;

		int yOffset = mUpAxis == Y_UP ? 1 : 2;
		int zOffset = mUpAxis == Y_UP ? 2 : 1;
		float zMultiplier = mUpAxis == Y_UP ? 1.0f : -1.0f;

		for (int i = 0; i < primitiveCount; i++)
		{
			if (primitiveType == PolyList)
			{
				pCount = polyListvCount[i];

				if (pCount != 3)
					cout << "Non-triangle primitives are unsupported at this time." << endl;
			}

			for (int p = 0; p < pCount; p++)
			{
				int currentIndex = baseIndex + p * primitiveInputStride;

				//Copy params if they exist
				vert.Position = vec3(accessorArray[0].source->data[pList[currentIndex + accessorArray[0].offset] * accessorArray[0].source->stride] * mScalingFactor, 
									 accessorArray[0].source->data[pList[currentIndex + accessorArray[0].offset] * accessorArray[0].source->stride + yOffset] * mScalingFactor, 
									 accessorArray[0].source->data[pList[currentIndex + accessorArray[0].offset] * accessorArray[0].source->stride + zOffset] * mScalingFactor * zMultiplier);

				if (accessorArray[1].source != NULL)
					vert.Normal =   vec3(accessorArray[1].source->data[pList[currentIndex + accessorArray[1].offset] * accessorArray[1].source->stride], 
										 accessorArray[1].source->data[pList[currentIndex + accessorArray[1].offset] * accessorArray[1].source->stride + yOffset], 
										 accessorArray[1].source->data[pList[currentIndex + accessorArray[1].offset] * accessorArray[1].source->stride + zOffset] * zMultiplier);

				if (accessorArray[2].source != NULL)
					vert.TexCoord = vec2(accessorArray[2].source->data[pList[currentIndex + accessorArray[2].offset] * accessorArray[2].source->stride],
										 accessorArray[2].source->data[pList[currentIndex + accessorArray[2].offset] * accessorArray[2].source->stride + 1]);

				if (accessorArray[3].source != NULL)
					vert.Tangent =  vec3(accessorArray[3].source->data[pList[currentIndex + accessorArray[3].offset] * accessorArray[3].source->stride], 
										 accessorArray[3].source->data[pList[currentIndex + accessorArray[3].offset] * accessorArray[3].source->stride + yOffset], 
										 accessorArray[3].source->data[pList[currentIndex + accessorArray[3].offset] * accessorArray[3].source->stride + zOffset] * zMultiplier);

				if (accessorArray[4].source != NULL)
					vert.Bitangent =vec3(accessorArray[4].source->data[pList[currentIndex + accessorArray[4].offset] * accessorArray[4].source->stride], 
										 accessorArray[4].source->data[pList[currentIndex + accessorArray[4].offset] * accessorArray[4].source->stride + yOffset], 
										 accessorArray[4].source->data[pList[currentIndex + accessorArray[4].offset] * accessorArray[4].source->stride + zOffset] * zMultiplier);

				//Push the vertex to the mesh
				subMesh->mesh.Vertices.push_back(vert);

				
				subMesh->mesh.Indices.push_back(i * pCount + p);
			/*	else
					subMesh->mesh.Indices.push_back((i + 1) * pCount - p - 1);*/
			}

			if (primitiveType == PolyList)
			{
				baseIndex += primitiveInputStride * polyListvCount[i];
			}
			else
				baseIndex += primitiveInputStride * pCount;
		}

		newModel->subMeshes.push_back(subMesh);

		primitiveElement = primitiveElement->NextSiblingElement();
	}

	//Clean up
	for (auto it = mMeshSources.begin(); it != mMeshSources.end(); ++it)
	{
		delete it->second;
	}

	mMeshSources.clear();
}

int COLLADALoader::getInputIndexStride(XMLElement* primitiveElement)
{
	int highestOffset = 0;
	XMLElement* inputElement = primitiveElement->FirstChildElement("input");

	while (inputElement != NULL)
	{
		if (inputElement->IntAttribute("offset") > highestOffset)
			highestOffset = inputElement->IntAttribute("offset");

		inputElement = inputElement->NextSiblingElement("input");
	}

	return highestOffset + 1;
}

void COLLADALoader::loadEffects()
{
	XMLElement* currentEffectElement = NULL;

	if (mSceneDocument.FirstChildElement()->FirstChildElement("library_effects"))
		currentEffectElement = mSceneDocument.FirstChildElement()->FirstChildElement("library_effects")->FirstChildElement("effect");

	while (currentEffectElement != NULL)
	{
		bool specularMaterial = false;
		Effect* currentEffect = new Effect();

		map<string, Surface> surfaceMap;
		map<string, Sampler> samplerMap;

		const char* id = currentEffectElement->Attribute("id");

		XMLElement* profileElement = currentEffectElement->FirstChildElement("profile_COMMON");

		if (profileElement == NULL)
		{
			cout << "Only profile_COMMON is supported at the moment. Effect: " << id << " uses " << currentEffectElement->FirstChildElement()->Value() << endl;
			throw -1; //Probably going to crash because effect will be missing anyways
			/*currentEffectElement = currentEffectElement->NextSiblingElement();
			continue;*/
		}

		//Load in surface and sampler references
		loadNewParamElements(profileElement, surfaceMap, samplerMap);

		XMLElement* techinqueElement = profileElement->FirstChildElement("technique");

		XMLElement* lightingElement = NULL;

		if (techinqueElement)
			lightingElement = techinqueElement->FirstChildElement("lambert");
		
		//Go through different types of lighting parameters to determine correct type
		if (lightingElement == NULL)
		{
			specularMaterial = true;
			lightingElement = techinqueElement->FirstChildElement("blinn");
		}
		if (lightingElement == NULL)
			lightingElement = techinqueElement->FirstChildElement("phong");
		if (lightingElement == NULL)
		{
			cout << "No valid material description found in effect " << id << endl;
			throw -1;
		}

		XMLElement* attributeElement = NULL;
		string surfaceId;
		string imageId;

		//Emission
		attributeElement = lightingElement->FirstChildElement("emission");
		if (attributeElement && attributeElement->FirstChildElement("color") != NULL)
			readColor(attributeElement->FirstChildElement("color"), currentEffect->emission);
		else if (attributeElement && attributeElement->FirstChildElement("texture") != NULL)
		{
			surfaceId = samplerMap[attributeElement->FirstChildElement("texture")->Attribute("texture")].surfaceId;
			imageId = surfaceMap[surfaceId].imageId;
			currentEffect->emissiveMapId = mImages[imageId].filePath;
		}

		//Ambient
		attributeElement = lightingElement->FirstChildElement("ambient");
		if (attributeElement && attributeElement->FirstChildElement("color") != NULL)
			readColor(attributeElement->FirstChildElement("color"), currentEffect->ambient);

		//Diffuse
		attributeElement = lightingElement->FirstChildElement("diffuse");
		if (attributeElement && attributeElement->FirstChildElement("color") != NULL)
			readColor(attributeElement->FirstChildElement("color"), currentEffect->diffuse);
		else if (attributeElement && attributeElement->FirstChildElement("texture") != NULL)
		{
			surfaceId = samplerMap[attributeElement->FirstChildElement("texture")->Attribute("texture")].surfaceId;
			imageId = surfaceMap[surfaceId].imageId;
			currentEffect->diffuseTextureId = mImages[imageId].filePath;
		}

		//Specular
		attributeElement = lightingElement->FirstChildElement("specular");
		if (attributeElement && attributeElement->FirstChildElement("color") != NULL)
			readColor(attributeElement->FirstChildElement("color"), currentEffect->specular);
		else if (attributeElement && attributeElement->FirstChildElement("texture") != NULL)
		{
			surfaceId = samplerMap[attributeElement->FirstChildElement("texture")->Attribute("texture")].surfaceId;
			imageId = surfaceMap[surfaceId].imageId;
			currentEffect->specularMapId = mImages[imageId].filePath;
		}
		else
			currentEffect->specular = vec4(0.0f, 0.0f, 0.0f, 1.0f);

		attributeElement = lightingElement->FirstChildElement("shininess");
		if (attributeElement && attributeElement->FirstChildElement("float") != NULL)
		{
			currentEffect->specularPower = (float)atof(attributeElement->FirstChildElement("float")->GetText());
		}

		//Checking for bump map
		XMLElement* extraElement = techinqueElement->FirstChildElement("extra");

		if (extraElement == NULL)
			extraElement = profileElement->FirstChildElement("extra");

		XMLElement* bumpMapElement = NULL;

		if (extraElement && extraElement->FirstChildElement("technique") && 
			extraElement->FirstChildElement("technique")->FirstChildElement("bump"))
		{
			bumpMapElement = extraElement->FirstChildElement("technique")->FirstChildElement("bump");
		}

		if (bumpMapElement && bumpMapElement->FirstChildElement("texture"))
		{
			surfaceId = samplerMap[bumpMapElement->FirstChildElement("texture")->Attribute("texture")].surfaceId;
			imageId = surfaceMap[surfaceId].imageId;
			currentEffect->bumpMapId = mImages[imageId].filePath;
		}

		mEffects[id] = currentEffect;

		currentEffectElement = currentEffectElement->NextSiblingElement();
	}
}

void COLLADALoader::loadNewParamElements(XMLElement* profileElement, map<string, Surface>& surfaceMap, map<string, Sampler>& samplerMap)
{
	XMLElement* paramElement = profileElement->FirstChildElement("newparam");

	while (paramElement != NULL)
	{
		const char* id = paramElement->Attribute("sid");

		if (paramElement->FirstChildElement("surface") != NULL)
		{
			Surface newSurface = { paramElement->FirstChildElement("surface")->FirstChildElement("init_from")->GetText() };
			surfaceMap[id] = newSurface;
		}
		else if (paramElement->FirstChildElement("sampler2D") != NULL)
		{
			Sampler newSampler = { paramElement->FirstChildElement("sampler2D")->FirstChildElement("source")->GetText() };
			samplerMap[id] = newSampler;
		}

		paramElement = paramElement->NextSiblingElement("newparam");
	}
}

void COLLADALoader::loadMaterials()
{
	XMLElement* materialLibElement = mSceneDocument.FirstChildElement()->FirstChildElement("library_materials");
	XMLElement* currentMaterialElement = NULL;

	if (materialLibElement)
		currentMaterialElement = materialLibElement->FirstChildElement("material");

	while (currentMaterialElement)
	{
		const char* id = currentMaterialElement->Attribute("id");

		XMLElement* instanceEffectElement = currentMaterialElement->FirstChildElement("instance_effect");

		if (instanceEffectElement)
		{
			const char* url = instanceEffectElement->Attribute("url");

			if (*url != '#')
				cout << "Invalid url in material " << id << endl;
			else
			{
				Material newMaterial = { (url + 1) };

				mMaterials[id] = newMaterial;
			}
		}

		currentMaterialElement = currentMaterialElement->NextSiblingElement("material");
	}
}

void COLLADALoader::loadImages()
{
	XMLElement* imageLibElement = mSceneDocument.FirstChildElement()->FirstChildElement("library_images");
	XMLElement* currentImageElement = NULL;

	if (imageLibElement != NULL)
		currentImageElement = imageLibElement->FirstChildElement("image");

	while (currentImageElement != NULL)
	{
		string id = currentImageElement->Attribute("id");

		if (currentImageElement->FirstChildElement("data") != NULL)
		{
			cout << "Image embedding is not currently supported. Image element: " << id << endl;
			throw -1;
		}

		mImages[id] = Image(currentImageElement->FirstChildElement("init_from")->GetText());

		currentImageElement = currentImageElement->NextSiblingElement("image");
	}
}

void COLLADALoader::loadSceneNodes()
{
	mpRootNode = new SceneNode();

	XMLElement* rootScene = mSceneDocument.FirstChildElement()->FirstChildElement("library_visual_scenes");
	XMLElement* sceneElement = NULL;

	if (rootScene)
		sceneElement = rootScene->FirstChildElement("visual_scene");

	while (sceneElement)
	{
		XMLElement* nodeElement = sceneElement->FirstChildElement("node");

		while (nodeElement)
		{
			loadNode(mpRootNode, sceneElement);

			nodeElement = nodeElement->NextSiblingElement("node");
		}

		sceneElement = sceneElement->NextSiblingElement("visual_scene");
	}
}

void COLLADALoader::loadNode(SceneNode* parent, XMLElement* nodeElement)
{
	XMLElement* geometryElement = nodeElement->FirstChildElement("instance_geometry");

	if (geometryElement)
	{
		const char* url = geometryElement->Attribute("url");
		if (*url != '#')
			cout << "Invalid url in node: " << nodeElement->Attribute("id") << endl;

		url++;
		parent->model = mModels[url];

		XMLElement* materialElement = NULL;

		if (geometryElement->FirstChildElement("bind_material") && 
			geometryElement->FirstChildElement("bind_material")->FirstChildElement("technique_common"))
		{
			materialElement = geometryElement->FirstChildElement("bind_material")->FirstChildElement("technique_common");
			materialElement = materialElement->FirstChildElement("instance_material");
		}

		if (materialElement)
		{
			const char* url = materialElement->Attribute("target") + 1;
			
			parent->effectId = mMaterials[url].effectId;
		}
	}

	XMLElement* childNodeElement = nodeElement->FirstChildElement("node");

	while (childNodeElement)
	{
		SceneNode* childNode = new SceneNode();
		parent->children.push_back(childNode);

		loadNode(childNode, childNodeElement);

		childNodeElement = childNodeElement->NextSiblingElement("node");
	}
}

void COLLADALoader::parse()
{
	loadAssetParameters();
	loadImages();
	loadEffects();
	loadMaterials();
	loadModels();
	loadSceneNodes();

	generateTangents();
}

const Model* COLLADALoader::getModel(const string& id) const
{
	auto find = mModels.find(id);

	if (find != mModels.end())
		return find->second;

	return NULL;
}

const Effect* COLLADALoader::getEffect(const string& id) const
{
	auto find = mEffects.find(id);

	if (find != mEffects.end())
		return find->second;

	return NULL;
}

void COLLADALoader::generateTangents()
{
	for (auto modelIt = mModels.begin(); modelIt != mModels.end(); ++modelIt)
	{
		if (modelIt->second == NULL)
			continue;

		for (auto meshIt = modelIt->second->subMeshes.begin(); meshIt != modelIt->second->subMeshes.end(); ++meshIt)
		{
			if ((*meshIt)->mesh.Vertices.size() > 0 && ((*meshIt)->mesh.Vertices[0].Tangent.length() == 0.0f && (*meshIt)->mesh.Vertices[0].Bitangent.length() == 0.0f))
				continue;

			for (unsigned i = 0; i < (*meshIt)->mesh.Indices.size() - 2;)
			{
				vec3 tangent(0.0f);
				vec3 binormal(0.0f);
				vec2 uVector(0.0f);
				vec2 vVector(0.0f);

				Vertex vert1 = (*meshIt)->mesh.Vertices[(*meshIt)->mesh.Indices[i]];
				Vertex vert2 = (*meshIt)->mesh.Vertices[(*meshIt)->mesh.Indices[i + 1]];
				Vertex vert3 = (*meshIt)->mesh.Vertices[(*meshIt)->mesh.Indices[i + 2]];

				vec3 tangentVector1 = vert2.Position - vert1.Position;
				vec3 tangentVector2 = vert3.Position - vert1.Position;

				uVector.x = vert2.TexCoord.x - vert1.TexCoord.x;
				uVector.y = vert3.TexCoord.x - vert1.TexCoord.x;

				vVector.x = vert2.TexCoord.y - vert1.TexCoord.y;
				vVector.y = vert3.TexCoord.y - vert1.TexCoord.y;

				float denominator = (uVector.x * vVector[1] - uVector[1] * vVector[0]);

				if (denominator != 0.0f)
				{
					denominator = 1.0f / denominator;

					tangent.x = (vVector[1] * tangentVector1.x - vVector[0] * tangentVector2[0]) * denominator;
					tangent.y = (vVector[1] * tangentVector1[1] - vVector[0] * tangentVector2[1]) * denominator;
					tangent.z = (vVector[1] * tangentVector1[2] - vVector[0] * tangentVector2[2]) * denominator;

					binormal.x = (uVector[0] * tangentVector2[0] - uVector[1] * tangentVector1[0]) * denominator;
					binormal.y = (uVector[0] * tangentVector2[1] - uVector[1] * tangentVector1[1]) * denominator;
					binormal.z = (uVector[0] * tangentVector2[2] - uVector[1] * tangentVector1[2]) * denominator;

					glm::normalize(tangent);
					glm::normalize(binormal);

					(*meshIt)->mesh.Vertices[(*meshIt)->mesh.Indices[i]].Tangent = tangent;
					(*meshIt)->mesh.Vertices[(*meshIt)->mesh.Indices[i]].Bitangent = binormal;
					(*meshIt)->mesh.Vertices[(*meshIt)->mesh.Indices[i + 1]].Tangent = tangent;
					(*meshIt)->mesh.Vertices[(*meshIt)->mesh.Indices[i + 1]].Bitangent = binormal;
					(*meshIt)->mesh.Vertices[(*meshIt)->mesh.Indices[i + 1]].Tangent = tangent;
					(*meshIt)->mesh.Vertices[(*meshIt)->mesh.Indices[i + 1]].Bitangent = binormal;
				}

				i += 3;
			}
		}
	}
}