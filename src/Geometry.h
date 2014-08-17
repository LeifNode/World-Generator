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