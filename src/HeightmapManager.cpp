#include "HeightmapManager.h"
#include "GameTimer.h"

HeightmapManager::HeightmapManager(int resolution, int cacheSize)
	:mResolution(resolution),
	mCacheSize(cacheSize)
{
	mpCache = new HeightmapCache(cacheSize, mResolution);
	mpGenerator = new HeightmapGenerator(mResolution);
}

HeightmapManager::~HeightmapManager()
{
	delete mpCache;
	delete mpGenerator;

	mpCache = NULL;
	mpGenerator = NULL;
}

void HeightmapManager::Initialize()
{
	mpCache->Initialize();
	mpGenerator->Initialize();
}

Texture* HeightmapManager::getHeightmap(const unsigned __int64& id, const dvec2& offset, double scale, const vec3& faceNormal)
{
	Texture* texture = mpCache->getTexture(id, true);

	if (texture != NULL)
		return texture;

	GameTimer timer;
	timer.Reset();
	timer.Start();

	texture = mpCache->getFreeTexture(id);

	mpGenerator->setGenerationTarget(texture);
	mpGenerator->generateHeightmap(offset, scale, faceNormal);

	timer.Tick();

	std::cout << "Generated id: " << id << " in " << (timer.DeltaTime() * 1000.0) << "MS "
		      << " Cache size: " << mpCache->getActiveCount() << " Free count: "
			  << mpCache->getInactiveCount() << std::endl;

	return texture;
}