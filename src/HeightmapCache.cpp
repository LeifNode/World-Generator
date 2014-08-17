#include "HeightMapCache.h"
#include "Texture.h"

HeightmapCache::HeightmapCache(int cacheSize, int textureResolution)
	:mCacheSize(cacheSize),
	mTexResolution(textureResolution)
{
}

HeightmapCache::~HeightmapCache()
{
	auto activeEnd = mActiveTextures.end();
	for (auto it = mActiveTextures.begin(); it != activeEnd; ++it)
	{
		delete it->second;
	}

	mActiveTextures.clear();

	auto freeEnd = mFreeTextures.end();
	for (auto it = mFreeTextures.begin(); it != freeEnd; ++it)
	{
		delete it->TexturePtr;
	}

	mFreeTextures.clear();
}

void HeightmapCache::Initialize()
{
	unsigned __int64 placeholderId = MAXULONGLONG;

	for (int i = 0; i < mCacheSize; i++)
	{
		//Allocating GL texture
		GLuint textureRef;

		glGenTextures(1, &textureRef);
		glBindTexture(GL_TEXTURE_2D, textureRef);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mTexResolution, mTexResolution, 0, GL_RGBA, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		//Place in list
		Texture* newTexture = new Texture();
		newTexture->mpTexture = textureRef;
		mFreeTextures.push_front(IDHeightmap(placeholderId, newTexture));
	}

	std::cout << "Height map cache allocated. Total size: " << ((float)(mCacheSize * mTexResolution * mTexResolution * 4.0f * 4.0f) / (1024.0f * 1024.0f)) << "MB" << std::endl;
}

bool HeightmapCache::hasActiveTexture(const unsigned __int64& id)
{
	return mActiveTextures.find(id) != mActiveTextures.end();
}

bool HeightmapCache::hasInactiveTexture(const unsigned __int64& id)
{
	auto freeEnd = mFreeTextures.end();
	for (auto it = mFreeTextures.begin(); it != freeEnd; ++it)
		if ((*it).ID == id)
			return true;

	return false;
}

bool HeightmapCache::unfreeInactiveTexture(const unsigned __int64& id)
{
	auto freeEnd = mFreeTextures.end();
	for (auto it = mFreeTextures.begin(); it != freeEnd; ++it)
	{
		if ((*it).ID == id)
		{
			mActiveTextures[id] = (*it).TexturePtr;

			mFreeTextures.erase(it);

			return true;
		}
	}

	return false;
}

bool HeightmapCache::hasFreeTexture()
{
	return mFreeTextures.size() > 0;
}

Texture* HeightmapCache::getFreeTexture(const unsigned __int64& newId)
{
	if (mFreeTextures.size() == 0)
	{
		freeTexture();
	}

	Texture* back = mFreeTextures.back().TexturePtr;
	mFreeTextures.pop_back();

	mActiveTextures[newId] = back;

	return back;
}

Texture* HeightmapCache::getTexture(const unsigned __int64& id, bool searchFreeTextures)
{
	if (mActiveTextures.find(id) != mActiveTextures.end())
	{
		mLastFetched.remove(id);
		mLastFetched.push_front(id);
		return mActiveTextures[id];
	}
	else if (searchFreeTextures)
	{
		auto freeEnd = mFreeTextures.end();
		for (auto it = mFreeTextures.begin(); it != freeEnd; ++it)
		{
			if ((*it).ID == id)
			{
				mLastFetched.remove(id);
				mLastFetched.push_front(id);

				Texture* texPtr = (*it).TexturePtr;

				mFreeTextures.erase(it);

				mActiveTextures[id] = texPtr;

				return texPtr;
			}
		}
	}

	return NULL;
}

void HeightmapCache::clearLastFetched()
{
	mLastFetched.clear();
}

void HeightmapCache::freeTexture()
{
	bool freed = false;

	while (!freed && mLastFetched.size() > 0)
	{
		unsigned __int64 id = mLastFetched.back();
		mLastFetched.pop_back();

		auto it = mActiveTextures.find(id);
		if (it != mActiveTextures.end())
		{
			Texture* texPtr = it->second;

			mActiveTextures.erase(it);

			mFreeTextures.push_front(IDHeightmap(id, texPtr));

			freed = true;
		}
	}

	if (!freed)
	{
		auto it = mActiveTextures.end();

		mFreeTextures.push_back(IDHeightmap(it->first, it->second));
		mActiveTextures.erase(it);

		std::cout << __FILE__ << " should have not reached this point " << __LINE__ << std::endl;
	}
}

bool HeightmapCache::freeTexture(const unsigned __int64& id)
{
	auto find = mActiveTextures.find(id);
	if (find != mActiveTextures.end())
	{
		mFreeTextures.push_front(IDHeightmap(find->first, find->second));
		mLastFetched.remove(find->first);

		mActiveTextures.erase(find);

		return true;
	}

	return false;
}