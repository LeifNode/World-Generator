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