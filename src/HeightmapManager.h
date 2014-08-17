#pragma once

#include "glStd.h"
#include "HeightmapCache.h"
#include "HeightmapGenerator.h"
#include "Texture.h"

class HeightmapManager
{
public:
	HeightmapManager(int resolution, int cacheSize);
	~HeightmapManager();

	void Initialize();

	Texture* getHeightmap(const unsigned __int64& id, const dvec2& offset, double scale, const vec3& faceNormal);

	bool freeTexture(const unsigned __int64& id) { return mpCache->freeTexture(id); }

	int getCacheSize() const { return mCacheSize; }

private:
	HeightmapCache* mpCache;
	HeightmapGenerator* mpGenerator;

	int mResolution;
	int mCacheSize;
};