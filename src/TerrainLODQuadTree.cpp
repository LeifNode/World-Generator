#include "TerrainLODQuadTree.h"

TerrainLODQuadTree::TerrainLODQuadTree(int maxLOD, float mostDetailedDistance, vec3 surfaceNormal, float planetRadius)
	:mMaxLOD(maxLOD),
	mMostDetailedDistance(mostDetailedDistance),
	mSurfaceNormal(surfaceNormal),
	mPlanetRadius(planetRadius)
{
	assert(maxLOD < 28);
	std::cout << "Max LOD: " << maxLOD << std::endl;
}

TerrainLODQuadTree::~TerrainLODQuadTree()
{
	delete [] mpLODDistanceArray;
	//delete [] mpIDFactorArray;

	clear();
}

void TerrainLODQuadTree::Initialize()
{
	float currentDetailDistance = mMostDetailedDistance;

	mpLODDistanceArray = new float[mMaxLOD]();

	for (int i = mMaxLOD - 1; i >= 0; i--)
	{
		mpLODDistanceArray[i] = currentDetailDistance;

		currentDetailDistance = currentDetailDistance * 2.0f;
	}

	/*unsigned long long currentIDFactor = 4;

	mpIDFactorArray = new unsigned long long[mMaxLOD];

	for (int i = mMaxLOD; i >= 0; i--)
	{
		currentIDFactor += i;

		mpIDFactorArray[i] = currentIDFactor;

		currentIDFactor *= 4;
	}*/

	mSurfaceNormalTensor[0] = vec3(1.0f, 0.0f, 0.0f);
	mSurfaceNormalTensor[1] = vec3(0.0f, 1.0f, 0.0f);
	mSurfaceNormalTensor[2] = vec3(0.0f, 0.0f, 1.0f);
}

vec3 TerrainLODQuadTree::projectOntoSphere(const vec3& pos) const
{
	vec3 posSqr = pos * pos;
	vec3 result;
	
	result.x = pos.x * sqrtf(1.0f - (posSqr.y / 2.0f) - (posSqr.z / 2.0f) + ((posSqr.y * posSqr.z) / 3.0f));
	result.y = pos.y * sqrtf(1.0f - (posSqr.z / 2.0f) - (posSqr.x / 2.0f) + ((posSqr.z * posSqr.x) / 3.0f));
	result.z = pos.z * sqrtf(1.0f - (posSqr.x / 2.0f) - (posSqr.y / 2.0f) + ((posSqr.x * posSqr.y) / 3.0f));
	
	return result;
}

dvec3 TerrainLODQuadTree::projectOntoSphere(const dvec3& pos) const
{
	dvec3 posSqr = pos * pos;
	dvec3 result;
	
	result.x = pos.x * sqrt(1.0 - (posSqr.y / 2.0) - (posSqr.z / 2.0) + ((posSqr.y * posSqr.z) / 3.0));
	result.y = pos.y * sqrt(1.0 - (posSqr.z / 2.0) - (posSqr.x / 2.0) + ((posSqr.z * posSqr.x) / 3.0));
	result.z = pos.z * sqrt(1.0 - (posSqr.x / 2.0) - (posSqr.y / 2.0) + ((posSqr.x * posSqr.y) / 3.0));
	
	return result;
}

void TerrainLODQuadTree::Update(const Camera& camera)
{
	//Test for intersections though they should not happen
	std::map<unsigned long long, int> testMap;

	for (auto it = mCurrentRenderingNodes.begin(); it != mCurrentRenderingNodes.end(); ++it)
	{
		if (testMap.find((*it)->getID()) != testMap.end())
			testMap[(*it)->getID()]++;
		else
			testMap[(*it)->getID()] = 1;
	}

	for (auto it = testMap.begin(); it != testMap.end(); ++it)
	{
		if (it->second > 1)
		{
			std::cout << "Intersection at ID: " << it->first << std::endl;
			throw -1;
		}
	}

	clear();

	//Select new valid nodes
	selectNodes(dvec3(camera.getPosition()) - dvec3(0.0f, -mPlanetRadius, 0.0f), dvec3(0.0f, 1.0f, 0.0f), 1.0f, 0, 0);

	//Print out stuff
	for (auto it = mCurrentRenderingNodes.begin(); it != mCurrentRenderingNodes.end(); ++it)
	{
		//std::cout << (*it)->getID() << ",";
	}

	//std::cout << mCurrentRenderingNodes.size() << std::endl;

	//std::cout<< "\n\n\n";
}

float TerrainLODQuadTree::calculateDistanceToCamera(const dvec3& cameraPosWorld, const dvec3& planeCoord)
{
	vec3 spherePosition = vec3(projectOntoSphere(planeCoord));

	vec3 dCameraPosition = vec3(cameraPosWorld.x, cameraPosWorld.y, cameraPosWorld.z);
	vec3 worldPosition = vec3(spherePosition.x, spherePosition.y, spherePosition.z) * mPlanetRadius;

	return glm::length(worldPosition - dCameraPosition);
}

mat3 TerrainLODQuadTree::getNormalTransform(const vec3& surfaceNormal)
{
	mat3 transformMatrix = mat3(1.0f);

	if (abs(surfaceNormal.x) == 1.0f)
	{
		vec3 temp = transformMatrix[0];
		transformMatrix[0] = transformMatrix[1];
		transformMatrix[1] = temp;
		transformMatrix *= surfaceNormal.x;
	}
	else if (abs(surfaceNormal.z) == 1.0f)
	{
		vec3 temp = transformMatrix[2];
		transformMatrix[2] = transformMatrix[1];
		transformMatrix[1] = temp;
		transformMatrix *= surfaceNormal.z;
	}
	else if (abs(surfaceNormal.y) == 1.0f)
	{
		transformMatrix *= surfaceNormal.y;
	}
	else
		throw -1; //Invalid normal passed

	return transformMatrix;
}

void TerrainLODQuadTree::selectNodes(const dvec3& cameraPosition, const dvec3& position, double scale, int LODLevel, unsigned __int64 currentId)
{
	TerrainNode* newNode = new TerrainNode();
	double halfScale = scale / 2.0f;

	if (LODLevel < mMaxLOD - 1)
	{
		dvec3 nextNodePosition;
		dvec3 offsetVector1 = dvec3(halfScale, 0.0f, halfScale);
		dvec3 offsetVector2 = dvec3(halfScale, 0.0f, -halfScale);

		float distanceToCamera = 0.0f;

		nextNodePosition = position - offsetVector1;
		distanceToCamera = calculateDistanceToCamera(cameraPosition, nextNodePosition);

		unsigned long long currentIdTemp = currentId;
		unsigned long long nextId = currentId;

		if (distanceToCamera < mpLODDistanceArray[LODLevel])
		{
			//nextId = 0 * mpIDFactorArray[LODLevel] + currentIdTemp;
			nextId = (0ULL << (3 * (mMaxLOD - LODLevel))) | currentIdTemp;
			selectNodes(cameraPosition, nextNodePosition, halfScale, LODLevel + 1, nextId);
			newNode->renderUL = false;
		}

		nextNodePosition = position + offsetVector1;
		distanceToCamera = calculateDistanceToCamera(cameraPosition, nextNodePosition);

		if (distanceToCamera < mpLODDistanceArray[LODLevel])
		{
			//nextId = 1 * mpIDFactorArray[LODLevel] + currentIdTemp;
			nextId = (1ULL << (2 * (mMaxLOD - LODLevel))) | currentIdTemp;
			selectNodes(cameraPosition, nextNodePosition, halfScale, LODLevel + 1, nextId);
			newNode->renderBR = false;
		}

		nextNodePosition = position - offsetVector2;
		distanceToCamera = calculateDistanceToCamera(cameraPosition, nextNodePosition);

		if (distanceToCamera < mpLODDistanceArray[LODLevel])
		{
			//nextId = 2 * mpIDFactorArray[LODLevel] + currentIdTemp;
			nextId = (2ULL << (2 * (mMaxLOD - LODLevel))) | currentIdTemp;
			selectNodes(cameraPosition, nextNodePosition, halfScale, LODLevel + 1, nextId);
			newNode->renderUR = false;
		}

		nextNodePosition = position + offsetVector2;
		distanceToCamera = calculateDistanceToCamera(cameraPosition, nextNodePosition);

		if (distanceToCamera < mpLODDistanceArray[LODLevel])
		{
			//nextId = 3 * mpIDFactorArray[LODLevel] + currentIdTemp;
			nextId = (3ULL << (2 * (mMaxLOD - LODLevel))) | currentIdTemp;
			selectNodes(cameraPosition, nextNodePosition, halfScale, LODLevel + 1, nextId);
			newNode->renderBL = false;
		}
	}
		
	if (newNode->renderUL || newNode->renderUR || newNode->renderBL || newNode->renderBR)
	{
		newNode->mOffset = dvec2(position.x, position.z);
		newNode->mScale = scale;
		newNode->mTransformMatrix = glm::translate(mat4(1.0f), vec3(position) - vec3(0.0f, 1.0f, 0.0f)) * glm::scale(mat4(1.0f), vec3((float)scale, 1.0f, (float)scale));
		//newNode->mId = currentId + LODLevel;
		newNode->mId = currentId | (4ULL << (2 * (mMaxLOD - LODLevel)) - 1);
		newNode->mLOD = LODLevel;

		

		mCurrentRenderingNodes.push_back(newNode);
	}
	else
		delete newNode;
}

void TerrainLODQuadTree::clear()
{
	for (auto it = mCurrentRenderingNodes.begin(); it != mCurrentRenderingNodes.end(); ++it)
	{
		delete (*it);
	}

	mCurrentRenderingNodes.clear();
}

TerrainLODQuadTree::TerrainNode::TerrainNode()
	:mTransformMatrix(1.0f),
	mId(0),
	mOffset(dvec2(0.0)),
	mScale(1.0)
{
	resetRenderParams();
}

TerrainLODQuadTree::TerrainNode::~TerrainNode()
{

}
