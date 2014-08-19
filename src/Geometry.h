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

struct ColoredVertex
{
	vec3 Position;
	vec3 Normal;
	vec4 Color;

	ColoredVertex() { }

	ColoredVertex(float x, float y, float z, float normX, float normY, float normZ)
		:Position(x, y, z),
		Normal(normX, normY, normZ),
		Color(1.0f, 1.0f, 1.0f, 1.0f)
	{
	}

	ColoredVertex(float x, float y, float z, float normX, float normY, float normZ, float colR, float colG, float colB, float colA)
		:Position(x, y, z),
		Normal(normX, normY, normZ),
		Color(colR, colG, colB, colA)
	{
	}
};

struct ColoredMesh
{
	std::vector<ColoredVertex> Vertices;
	std::vector<GLuint> Indices;
};

struct Vertex
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoord;
	vec3 Tangent;
	vec3 Bitangent;

	Vertex()
	{ ZeroMemory(this, sizeof(this)); }

	Vertex(const vec3& position, const vec3& normal)
		:Position(position),
		Normal(normal)
	{
	}

	Vertex(const vec3& position, const vec3& normal, const vec2& texCoord)
		:Position(position),
		Normal(normal),
		TexCoord(texCoord)
	{
	}

	Vertex(const vec3& position, const vec3& normal, const vec2& texCoord, const vec3& tangent, const vec3& bitangent)
		:Position(position),
		Normal(normal),
		TexCoord(texCoord),
		Tangent(tangent),
		Bitangent(bitangent)
	{
	}
};

struct Mesh
{
	std::vector<Vertex> Vertices;
	std::vector<GLuint> Indices;
};

class GeometryGenerator
{
public:
	static void GenerateBox(float width, float height, float depth, ColoredMesh& mesh);
	static void GenerateDisk(float radius, int sliceCount, ColoredMesh& mesh);
	static void GeneratePlane(float width, float depth, unsigned c, unsigned r, ColoredMesh& mesh);
	static void GeneratePlane(float width, float depth, unsigned c, unsigned r, Mesh& mesh);
	static void GenerateSphere(float radius, int sliceCount, int stackCount, Mesh& mesh);
};