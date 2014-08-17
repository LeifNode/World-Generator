#include "TextureManager.h"
#include "TGALoader.h"

using namespace std;

TextureManager::TextureManager()
{

}

TextureManager::~TextureManager()
{
	clear();
}

void TextureManager::clear()
{
	auto end = mTextureMap.end();
	for (auto it = mTextureMap.begin(); it != end; ++it)
	{
		delete it->second;
	}

	mTextureMap.clear();
}

bool TextureManager::loadTexture(const char* path)
{
	string extension = getExtensionFromPath(path);

	auto find = mTextureMap.find(path);

	if (find != mTextureMap.end())
	{
		//cout << "Skipped loading texture from: " << find->first << " since it is already loaded." << endl;
		return true;
	}

	if (extension == ".dds")
		return loadDDSTexture(path);
	else if (extension == ".tga")
		return loadTGATexture(path);
	else
	{
		cout << "Could not load file: " << path << " because it has an unsupported extension." << endl;
	}

	return false;
}

Texture* TextureManager::getTexture(const char* path)
{
	auto texture = mTextureMap.find(path);

	if (texture != mTextureMap.end())
		return texture->second;

	return NULL;
}

bool TextureManager::loadDDSTexture(const char* path)
{
	nv_dds::CDDSImage image;

	if (!image.load(path))
	{
		//cout << "Could not load texture from " << path << endl;
		return false;
	}

	GLuint textureRef = 0;
	bool hasMipmaps = false;

	if (image.is_compressed())
	{
		if (image.is_cubemap())
		{
			cout << "Compressed cube maps not supported at this time" << endl;
			return false;
		}

		textureRef = loadCompressedDDSTexture(image, hasMipmaps);
	}
	else
	{
		if (image.is_cubemap())
			textureRef = loadCubeMapDDSTexture(image, hasMipmaps);
		else
			textureRef = loadUncompressedDDSTexture(image, hasMipmaps);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (hasMipmaps)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	Texture* newTex = new Texture();
	newTex->mpTexture = textureRef;

	mTextureMap[path] = newTex;

	return true;
}

GLuint TextureManager::loadCubeMapDDSTexture(nv_dds::CDDSImage& image, bool& hasMipmaps)
{
	GLuint textureRef;
	GLenum target;

	glGenTextures(1, &textureRef);
	glEnable(GL_TEXTURE_CUBE_MAP_ARB);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, textureRef);

	for (int n = 0; n < 6; n++)
	{
		target = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB+n;

		glTexImage2D(target, 0, image.get_components(), image.get_cubemap_face(n).get_width(),
			image.get_cubemap_face(n).get_height(), 0, image.get_format(), GL_UNSIGNED_BYTE, image.get_cubemap_face(n));

		for (unsigned i = 0; i < image.get_cubemap_face(n).get_num_mipmaps(); i++)
		{
			hasMipmaps = true;
			glTexImage2D(target, i+1, image.get_components(), 
				image.get_cubemap_face(n).get_mipmap(i).get_width(), 
				image.get_cubemap_face(n).get_mipmap(i).get_height(), 0,
				image.get_format(), GL_UNSIGNED_BYTE, image.get_cubemap_face(n).get_mipmap(i));
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (hasMipmaps)
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	else
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureRef;
}

GLuint TextureManager::loadUncompressedDDSTexture(nv_dds::CDDSImage& image, bool& hasMipmaps)
{
	GLuint textureRef;

	glGenTextures(1, &textureRef);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureRef);

	glTexImage2D(GL_TEXTURE_2D, 0, image.get_components(), image.get_width(), image.get_height(),
				 0, image.get_format(), GL_UNSIGNED_BYTE, image);

	for (unsigned i = 0; i < image.get_num_mipmaps(); i++)
	{
		hasMipmaps = true;
		glTexImage2D(GL_TEXTURE_2D, i+1, image.get_components(), 
					 image.get_mipmap(i).get_width(), image.get_mipmap(i).get_height(),
					 0, image.get_format(), GL_UNSIGNED_BYTE, image.get_mipmap(i));
	}

	return textureRef;
}

GLuint TextureManager::loadCompressedDDSTexture(nv_dds::CDDSImage& image, bool& hasMipmaps)
{
	GLuint textureRef;

	glGenTextures(1, &textureRef);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureRef);

	glCompressedTexImage2DARB(GL_TEXTURE_2D, 0, image.get_format(), 
						      image.get_width(), image.get_height(),
							  0, image.get_size(), image);

	for (unsigned i = 0; i < image.get_num_mipmaps(); i++)
	{
		hasMipmaps = true;
		nv_dds::CSurface mipmap = image.get_mipmap(i);

		glCompressedTexImage2DARB(GL_TEXTURE_2D, i+1, image.get_format(), 
					 mipmap.get_width(), mipmap.get_height(),
					 0, mipmap.get_size(), mipmap);
	}

	return textureRef;
}

bool TextureManager::loadTGATexture(const char* path)
{
	NS_TGALOADER::IMAGE image;

	if (!image.LoadTGA(path))
	{
		//cout << "Could not load texture from " << path << endl;
		return false;
	}

	GLuint textureRef;

	glGenTextures(1, &textureRef);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureRef);

	glTexImage2D(GL_TEXTURE_2D, 0, 4, image.getWidth(), image.getHeight(), 0, GL_BGRA, GL_UNSIGNED_BYTE, image.getDataForOpenGL());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	Texture* newTex = new Texture();
	newTex->mpTexture = textureRef;
	
	mTextureMap[path] = newTex;

	return true;
}

std::string TextureManager::getExtensionFromPath(const char* path)
{
	const char* currentPtr = path;

	while (*currentPtr != '\0')
		currentPtr++;

	while (*currentPtr != '.' && currentPtr != path)
		currentPtr--;

	return std::string(currentPtr);
}