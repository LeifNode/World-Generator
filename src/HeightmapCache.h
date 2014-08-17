#pragma once

#include "glStd.h"
#include <list>

class Texture;

class HeightmapCache
{
	struct IDHeightmap
	{
		unsigned __int64 ID;
		Texture* TexturePtr;

		IDHeightmap(const unsigned __int64& id, Texture* texture)
			:ID(id),
			TexturePtr(texture) { }
		~IDHeightmap() { }
	};

public:
	HeightmapCache(int cacheSize, int textureResolution);
	~HeightmapCache();

	void Initialize();

	int getActiveCount() const { return mActiveTextures.size(); }
	int getInactiveCount() const { return mFreeTextures.size(); }

	bool hasActiveTexture(const unsigned __int64& id);
	bool hasInactiveTexture(const unsigned __int64& id);
	bool unfreeInactiveTexture(const unsigned __int64& id);
	bool hasFreeTexture();

	Texture* getFreeTexture(const unsigned __int64& newId);
	Texture* getTexture(const unsigned __int64& id, bool searchFreeTextures = true);

	void clearLastFetched();
	void freeTexture();
	bool freeTexture(const unsigned __int64& id);

private:
	//ID-Texture reference
	std::map<unsigned __int64, Texture*> mActiveTextures;
	std::list<IDHeightmap> mFreeTextures;
	std::list<unsigned __int64> mLastFetched;

	int mCacheSize;
	int mTexResolution;
};