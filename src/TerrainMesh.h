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

class Texture;

struct TerrainVertex
{
	vec3 position;
	vec2 texCoord;

	TerrainVertex()
	{ ZeroMemory(this, sizeof(this)); }

	TerrainVertex(const vec3& pos, const vec2& tex)
		:position(pos),
		texCoord(tex)
	{}
};

class TerrainMesh
{
public:
	TerrainMesh(int resolution, float scale);
	~TerrainMesh();

	void Initialize();

	void Render();

	bool renderUL, renderUR, renderBL, renderBR;

	void resetRenderParams() { renderUL = renderUR = renderBL = renderBR = true; }

	void setCurrentTexture(Texture* texture) { mpCurrentTexture = texture; }

	int getResolution() const { return mResolution; }

private:
	void generateMesh(int resolution, float scale, std::vector<TerrainVertex>& vertexOut, std::vector<std::vector<GLuint>>& indexOut);

private:
	GLuint mpVertexArray;
	GLuint mpVertexBuffer;

	GLuint* mpIndexBuffers;

	Texture* mpCurrentTexture;

	int mResolution;
	float mScale;
	int mIndexCount;
};