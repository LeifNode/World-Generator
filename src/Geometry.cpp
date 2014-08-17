#include "Geometry.h"

void GeometryGenerator::GenerateBox(float width, float height, float depth, ColoredMesh& mesh)
{
	ColoredVertex v[24];

	float w2 = 0.5f*width;
	float h2 = 0.5f*height;
	float d2 = 0.5f*depth;
    
	//Front
	v[0] = ColoredVertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 0.0f, 0.5f, 0.0f, 1.0f);
	v[1] = ColoredVertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f);
	v[2] = ColoredVertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f);
	v[3] = ColoredVertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 0.0f, 0.5f, 0.0f, 1.0f);

	//Back
	v[4] = ColoredVertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.0f, 1.0f);
	v[5] = ColoredVertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.0f, 1.0f);
	v[6] = ColoredVertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f);
	v[7] = ColoredVertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f);

	//Top
	v[8]  = ColoredVertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 0.9f, 0.2f, 0.0f, 1.0f);
	v[9]  = ColoredVertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 0.9f, 0.2f, 0.0f, 1.0f);
	v[10] = ColoredVertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f);
	v[11] = ColoredVertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f);

	//Bottom
	v[12] = ColoredVertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, 0.0f, 0.5f, 0.0f, 1.0f);
	v[13] = ColoredVertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, 0.0f, 0.5f, 0.0f, 1.0f);
	v[14] = ColoredVertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, 0.0f, 0.5f, 0.0f, 1.0f);
	v[15] = ColoredVertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, 0.0f, 0.5f, 0.0f, 1.0f);

	//Left
	v[16] = ColoredVertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 1.0f);
	v[17] = ColoredVertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f);
	v[18] = ColoredVertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f);
	v[19] = ColoredVertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 1.0f);

	//Right
	v[20] = ColoredVertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 1.0f);
	v[21] = ColoredVertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f);
	v[22] = ColoredVertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f);
	v[23] = ColoredVertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 1.0f);

	mesh.Vertices.assign(&v[0], &v[24]);

	GLuint i[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7]  = 5; i[8]  = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] =  9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	mesh.Indices.assign(&i[0], &i[36]);
}

void GeometryGenerator::GenerateDisk(float radius, int sliceCount, ColoredMesh& mesh)
{
	mesh.Vertices.clear();
	mesh.Indices.clear();

	float dAngle = MathUtils::TwoPi / sliceCount;

	mesh.Vertices.push_back(ColoredVertex(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f)); //Center

	for (int i = 0; i <= sliceCount; i++)
	{
		float x = radius * cosf(i * dAngle);
		float z = radius * sinf(i * dAngle);

		mesh.Vertices.push_back(ColoredVertex(x, 0.0f, z, 0.0f, 1.0f, 0.0f, 1.0f * i * dAngle, 0.0f, 0.0f, 1.0f));
	}

	for (int i = 0; i < sliceCount; i++)
	{
		mesh.Indices.push_back(0);
		mesh.Indices.push_back(i + 2);
		mesh.Indices.push_back(i + 1);
	}
}

void GeometryGenerator::GeneratePlane(float width, float depth, unsigned c, unsigned r, ColoredMesh& mesh)
{
	mesh.Vertices.clear();
	mesh.Indices.clear();

	float halfWidth = width / 2.0f;
	float halfDepth = depth / 2.0f;

	float dx = width / (float)c;
	float dz = depth / (float)r;

	for (unsigned x = 0; x <= r; x++)
	{
		float currentX = -halfWidth + x * dx;

		for (unsigned z = 0; z <= c; z++)
		{
			mesh.Vertices.push_back(ColoredVertex(currentX, 0.0f, -halfDepth + z * dz, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f));
		}
	}

	for (unsigned x = 0; x < r; x++)
	{
		for (unsigned z = 0; z < c; z++)
		{
			mesh.Indices.push_back(x + z * c);
			mesh.Indices.push_back(x + (z + 1) * c);
			mesh.Indices.push_back(x + 1 + (z + 1) * c);
			mesh.Indices.push_back(x + z * c);
			mesh.Indices.push_back(x + 1 + (z + 1) * c);
			mesh.Indices.push_back(x + 1 + z * c);
		}
	}
}

void GeometryGenerator::GeneratePlane(float width, float depth, unsigned c, unsigned r, Mesh& mesh)
{
	mesh.Vertices.clear();
	mesh.Indices.clear();

	float halfWidth = width / 2.0f;
	float halfDepth = depth / 2.0f;

	float dx = width / (float)c;
	float dz = depth / (float)r;

	for (unsigned x = 0; x < r; x++)
	{
		float currentX = -halfWidth + x * dx;

		for (unsigned z = 0; z < c; z++)
		{
			float texX = z / (float)(c + 1);
			float texY = x / (float)(r + 1);
			mesh.Vertices.push_back(Vertex(vec3(currentX, 0.0f, -halfDepth + z * dz), vec3(0.0f, 1.0f, 0.0f), vec2(texX, texY), vec3(0.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f)));
		}
	}

	for (unsigned x = 0; x < r - 1; x++)
	{
		for (unsigned z = 0; z < c - 1; z++)
		{
			mesh.Indices.push_back(x + z * c);
			mesh.Indices.push_back(x + 1 + z * c);
			mesh.Indices.push_back(x + 1 + (z + 1) * c);

			mesh.Indices.push_back(x + z * c);
			mesh.Indices.push_back(x + 1 + (z + 1) * c);
			mesh.Indices.push_back(x + (z + 1) * c);
		}
	}
}

void GeometryGenerator::GenerateSphere(float radius, int sliceCount, int stackCount, Mesh& mesh)
{
mesh.Vertices.clear();
	mesh.Indices.clear();

	int reserveCount = sliceCount * stackCount + 2;

	mesh.Vertices.reserve(reserveCount);

	//
	// Compute the vertices stating at the top pole and moving down the stacks.
	//

	// Poles: note that there will be texture coordinate distortion as there is
	// not a unique point on the texture map to assign to the pole when mapping
	// a rectangular texture onto a sphere.
	Vertex topVertex(vec3(0.0f, +radius, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f));
	Vertex bottomVertex(vec3(0.0f, -radius, 0.0f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 1.0f));

	mesh.Vertices.push_back( topVertex );

	float phiStep   = MathUtils::Pi/stackCount;
	float thetaStep = (MathUtils::Pi * 2.0f)/sliceCount;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for(int i = 1; i <= stackCount-1; ++i)
	{
		float phi = i*phiStep;

		// Vertices of ring.
		for(int j = 0; j <= sliceCount; ++j)
		{
			float theta = j*thetaStep;

			Vertex v;

			// spherical to cartesian
			v.Position.x = radius*sinf(phi)*cosf(theta);
			v.Position.y = radius*cosf(phi);
			v.Position.z = radius*sinf(phi)*sinf(theta);

			v.Tangent.x = -radius*sinf(phi)*sinf(theta);
			v.Tangent.y = 0.0f;
			v.Tangent.z = +radius*sinf(phi)*cosf(theta);

			v.Bitangent = -glm::normalize(glm::cross(v.Normal, v.Tangent));

			vec3 p = v.Position;
			v.Normal= glm::normalize(p);

			v.TexCoord.x = theta / MathUtils::Pi * 2.0f;
			v.TexCoord.y = phi / MathUtils::Pi;

			mesh.Vertices.push_back( v );
		}
	}

	mesh.Vertices.push_back( bottomVertex );

	//
	// Compute indices for top stack.  The top stack was written first to the vertex buffer
	// and connects the top pole to the first ring.
	//

	for(int i = 1; i <= sliceCount; ++i)
	{
		mesh.Indices.push_back(0);
		mesh.Indices.push_back(i+1);
		mesh.Indices.push_back(i);
	}
	
	//
	// Compute indices for inner stacks (not connected to poles).
	//

	// Offset the indices to the index of the first vertex in the first ring.
	// This is just skipping the top pole vertex.
	int baseIndex = 1;
	int ringVertexCount = sliceCount+1;
	for(int i = 0; i < stackCount-2; ++i)
	{
		for(int j = 0; j < sliceCount; ++j)
		{
			mesh.Indices.push_back(baseIndex + i*ringVertexCount + j);
			mesh.Indices.push_back(baseIndex + i*ringVertexCount + j+1);
			mesh.Indices.push_back(baseIndex + (i+1)*ringVertexCount + j);

			mesh.Indices.push_back(baseIndex + (i+1)*ringVertexCount + j);
			mesh.Indices.push_back(baseIndex + i*ringVertexCount + j+1);
			mesh.Indices.push_back(baseIndex + (i+1)*ringVertexCount + j+1);
		}
	}

	//
	// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	// and connects the bottom pole to the bottom ring.
	//

	// South pole vertex was added last.
	int southPoleIndex = mesh.Vertices.size()-1;

	// Offset the indices to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;
	
	for(int i = 0; i < sliceCount; ++i)
	{
		mesh.Indices.push_back(southPoleIndex);
		mesh.Indices.push_back(baseIndex+i);
		mesh.Indices.push_back(baseIndex+i+1);
	}
}