#pragma once

#include "glStd.h"
#include "Texture.h"
#include "DDSLoader.h"

class TextureManager
{
public:
	static TextureManager& getInstance()
    {
        static TextureManager instance;

        return instance;
    }

	void clear();

	bool loadTexture(const char* path);
	bool loadCubeMap(const char* path);

	Texture* getTexture(const char* path);

private:
	TextureManager();
	~TextureManager();

	bool loadDDSTexture(const char* path);
	GLuint loadCubeMapDDSTexture(nv_dds::CDDSImage& image, bool& hasMipmaps);
	GLuint loadUncompressedDDSTexture(nv_dds::CDDSImage& image, bool& hasMipmaps);
	GLuint loadCompressedDDSTexture(nv_dds::CDDSImage& image, bool& hasMipmaps);

	bool loadTGATexture(const char* path);

	std::string getExtensionFromPath(const char* path);

private:
	std::map<std::string, Texture*> mTextureMap;
};