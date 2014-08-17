#include "Texture.h"

Texture::Texture()
	:mpTexture(0)
{
}

Texture::~Texture()
{
	if (mpTexture)
	{
		glDeleteTextures(1, &mpTexture);
		mpTexture = 0U;
	}
}

void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, mpTexture);
}

void Texture::bindCube()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, mpTexture);
}