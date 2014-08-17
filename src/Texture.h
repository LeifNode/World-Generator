#pragma once

#include "glStd.h"

class Texture
{
	friend class TextureManager;
	friend class HeightmapGenerator;
	friend class HeightmapCache;

public:
	void bind();
	void bindCube();

	GLuint getReference() const { return mpTexture; }

private:
	Texture();
	~Texture();

private:
	GLuint mpTexture;
};