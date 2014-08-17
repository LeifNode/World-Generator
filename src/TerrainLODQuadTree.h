#pragma once

#include "glStd.h"
#include "Camera.h"

class TerrainLODQuadTree
{
public:
	class TerrainNode;
	//The surface normal determines what face of a cube this surface is on
	TerrainLODQuadTree(int maxLOD, float mostDetailedDistance, vec3 surfaceNormal, float planetRadius = 6378100.0f);
	~TerrainLODQuadTree();

	void Initialize();

	std::vector<TerrainNode*> mCurrentRenderingNodes;

	void Update(const Camera& camera);

	void clear();

private:
	vec3 projectOntoSphere(const vec3& position) const;
	dvec3 projectOntoSphere(const dvec3& position) const;
	float TerrainLODQuadTree::calculateDistanceToCamera(const dvec3& cameraPosWorld, const dvec3& planeCoord);
	mat3 getNormalTransform(const vec3& surfaceNormal);

	void selectNodes(const dvec3& cameraPos, const dvec3& position, double scale, int LODLevel, unsigned __int64 currentId);

private:
	int mMaxLOD;
	float mMostDetailedDistance;
	float mPlanetRadius;

	float* mpLODDistanceArray;
	//unsigned long long* mpIDFactorArray;

	vec3 mSurfaceNormal;
	mat3 mSurfaceNormalTensor; //Allows the plane to be the face of a box, WIP

public:
	class TerrainNode
	{
		friend class TerrainLODQuadTree;
	public:
		TerrainNode();
		~TerrainNode();

		unsigned __int64 getID() const { return mId; }
		int getLOD() const { return mLOD; }

		mat4 getTransform() const { return mTransformMatrix; }
		dvec2 getOffset() const { return mOffset; }
		double getScale() const { return mScale; }

		float getMinimumDistance() const { return mMinimumDistance; }
		float getMaximumDistance() const { return mMaximumDistance; }

		bool renderUL, renderUR, renderBL, renderBR;

		void resetRenderParams() { renderUL = renderUR = renderBL = renderBR = true; }

	private:
		int mLOD;

		dvec2 mOffset;
		double mScale;
		mat4 mTransformMatrix;
		float mMinimumDistance;
		float mMaximumDistance;

		unsigned __int64 mId;
	};
};