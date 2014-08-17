#include "TerrainRenderer.h"
#include "LoadShaders.h"
#include "TerrainMesh.h"
#include "Camera.h"
#include "TerrainLODQuadTree.h"
#include "GrassRenderer.h"
#include "AtmosphereRenderer.h"

TerrainRenderer::TerrainRenderer(float planetRadius)
	:mPlanetRadius(planetRadius),
	mpQuadTree(NULL),
	mpTerrainUniformBuffer(0),
	mpAtmosphereUniformBuffer(0),
	mTerrainDimensionLocation(0)
{
	int resolution = 64;

	mpMesh = new TerrainMesh(resolution, 2.0f);
	mpQuadTree = new TerrainLODQuadTree(19, 25.0, vec3(0.0f, 1.0f, 0.0f));
	mpHeightmapManager = new HeightmapManager(resolution * 8, 120);
	mpGrassRenderer = new GrassRenderer();
}

TerrainRenderer::~TerrainRenderer()
{
	delete mpMesh;
	mpMesh =  nullptr;

	delete mpQuadTree;
	mpQuadTree = nullptr;

	delete mpHeightmapManager;
	mpHeightmapManager = nullptr;

	delete mpGrassRenderer;
	mpGrassRenderer = nullptr;

	glDeleteBuffers(1, &mpTerrainUniformBuffer);
	mpTerrainUniformBuffer = 0;

	glDeleteBuffers(1, &mpAtmosphereUniformBuffer);
	mpAtmosphereUniformBuffer = 0;

	glDeleteProgram(mpTerrainProgram);
	mpTerrainProgram = 0;
}

void TerrainRenderer::Initialize()
{
	//Set up mesh
	mpMesh->Initialize();
	mpQuadTree->Initialize();
	mpHeightmapManager->Initialize();
	//mpGrassRenderer->Initialize();

	//Compile shader
	ShaderInfo terrainShaderInfo[] = 
	{
		{ GL_VERTEX_SHADER, "Shaders/terrain_vs.glsl" },
		{ GL_FRAGMENT_SHADER, "Shaders/terrain_fs.glsl" },
		{ GL_NONE, NULL },
	};

	mpTerrainProgram = LoadShaders(terrainShaderInfo);
	glUseProgram(mpTerrainProgram);

	GLuint perFrameUniformIndex = glGetUniformBlockIndex(mpTerrainProgram, "UniformPerFrameBlock");
	glUniformBlockBinding(mpTerrainProgram, perFrameUniformIndex, 1);

	GLuint perObjectUniformIndex = glGetUniformBlockIndex(mpTerrainProgram, "UniformPerObjectBlock");
	glUniformBlockBinding(mpTerrainProgram, perObjectUniformIndex, 2);

	//Initialize terrain uniform buffer
	GLuint perTerrainPatchIndex = glGetUniformBlockIndex(mpTerrainProgram, "TerrainUniformBuffer");
	glUniformBlockBinding(mpTerrainProgram, perTerrainPatchIndex, 3);

	GLuint atmosphereUniformIndex = glGetUniformBlockIndex(mpTerrainProgram, "UniformAtmosphereBlock");
	glUniformBlockBinding(mpTerrainProgram, atmosphereUniformIndex, 4);

	//mTerrainDimensionLocation = glGetUniformLocation(mpTerrainProgram, "TerrainDimensions");

	glGenBuffers(1, &mpTerrainUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, mpTerrainUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(TerrainUniformBuffer), NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, mpTerrainUniformBuffer);

	float m_Kr = 0.0025f;		// Rayleigh scattering constant
	float m_Kr4PI = m_Kr*4.0f*MathUtils::Pi;
	float m_Km = 0.0010f;		// Mie scattering constant
	float m_Km4PI = m_Km*4.0f*MathUtils::Pi;
	float m_ESun = 20.0f;		// Sun brightness constant
	float m_g = -0.990f;		// The Mie phase asymmetry factor
	float m_fExposure = 2.0f;
	float m_fScale = 1 / (atmosphereScale - worldScale);
	float m_fRayleighScaleDepth = 0.25f;
	float m_fMieScaleDepth = 0.1f;

	float wavelengths[3];
	wavelengths[0] = 0.650f;		// 650 nm for red
	wavelengths[1] = 0.570f;		// 570 nm for green
	wavelengths[2] = 0.475f;		// 475 nm for blue
	wavelengths[0] = powf(wavelengths[0], 4.0f);
	wavelengths[1] = powf(wavelengths[1], 4.0f);
	wavelengths[2] = powf(wavelengths[2], 4.0f);


	PerAtmosphereBlock atmosphereBlock;
	atmosphereBlock.v3InvWavelength = vec3(1.0f/wavelengths[0], 1.0f/wavelengths[1], 1.0f/wavelengths[2]);
	atmosphereBlock.fInnerRadius = worldScale;
	atmosphereBlock.fInnerRadius2 = worldScale * worldScale;
	atmosphereBlock.fOuterRadius = atmosphereScale;
	atmosphereBlock.fOuterRadius2 = atmosphereScale * atmosphereScale;
	atmosphereBlock.fKrESun = m_Kr * m_ESun;
	atmosphereBlock.fKmESun = m_Km * m_ESun;
	atmosphereBlock.fKr4PI = m_Kr4PI;
	atmosphereBlock.fKm4PI = m_Km4PI;
	atmosphereBlock.fScale = m_fScale;
	atmosphereBlock.fScaleDepth = m_fRayleighScaleDepth;
	atmosphereBlock.fScaleOverScaleDepth = m_fScale / m_fRayleighScaleDepth;
	atmosphereBlock.g = m_g;
	atmosphereBlock.g2 = m_g * m_g;

	glGenBuffers(1, &mpAtmosphereUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, mpAtmosphereUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(TerrainUniformBuffer), &atmosphereBlock, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 4, mpAtmosphereUniformBuffer);
}

void TerrainRenderer::Update(const Camera& camera, float dt)
{
	mpGrassRenderer->Update(dt);

	std::map<unsigned __int64, char> oldNodeIds;

	for (auto it = mpQuadTree->mCurrentRenderingNodes.begin(); it != mpQuadTree->mCurrentRenderingNodes.end(); ++it)
	{
		oldNodeIds[(*it)->getID()] = 1;
	}

	//Select new nodes based on camera position
	mpQuadTree->Update(camera);

	//Mark unused nodes as free
	std::list<unsigned __int64> freeNodeIds;

	auto currentNodesEnd = mpQuadTree->mCurrentRenderingNodes.end();
	auto oldNodesEnd = oldNodeIds.end();
	for (auto it = mpQuadTree->mCurrentRenderingNodes.begin(); it != currentNodesEnd; ++it)
		if (oldNodeIds.find((*it)->getID()) == oldNodesEnd)
			freeNodeIds.push_back((*it)->getID());

	for (auto it = freeNodeIds.begin(); it != freeNodeIds.end(); ++it)
		mpHeightmapManager->freeTexture((*it));

	//Generate height maps
	for (auto it = mpQuadTree->mCurrentRenderingNodes.begin(); it != currentNodesEnd; ++it)
		mpHeightmapManager->getHeightmap((*it)->getID(), (*it)->getOffset(), (*it)->getScale(), vec3(0.0f, 1.0f, 0.0f));
}

void TerrainRenderer::Render(const PerFrameBlock& perFrame, GLuint perObjectBufferRef, GLuint lastProgramReference)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(mpTerrainProgram);

	mat4 terrainWorld = glm::scale(mat4(1.0f), vec3(1.0));//, vec3(0.0f, -mPlanetRadius, 0.0f));
	mat4 terrainMVP = perFrame.ViewProj * terrainWorld;

	PerObjectBlock objectBlock;
	objectBlock.World = terrainWorld;
	objectBlock.WorldViewProj = terrainMVP;
	objectBlock.WorldInvTranspose = glm::inverse(glm::transpose(terrainWorld));

	glBindBuffer(GL_UNIFORM_BUFFER, perObjectBufferRef);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PerObjectBlock), &objectBlock, GL_STATIC_DRAW);

	glBindBufferBase(GL_UNIFORM_BUFFER, 3, mpTerrainUniformBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 4, mpAtmosphereUniformBuffer);

	//glUniform2i(mTerrainDimensionLocation, mpMesh->getResolution(), mpMesh->getResolution());

	for (auto it = mpQuadTree->mCurrentRenderingNodes.begin(); it != mpQuadTree->mCurrentRenderingNodes.end(); ++it)
	{
		mpMesh->renderUL = (*it)->renderUL;
		mpMesh->renderUR = (*it)->renderUR;
		mpMesh->renderBL = (*it)->renderBL;
		mpMesh->renderBR = (*it)->renderBR;

		mpMesh->setCurrentTexture(mpHeightmapManager->getHeightmap((*it)->getID(), (*it)->getOffset(), (*it)->getScale(), vec3(0.0f, 1.0f, 0.0f)));

		TerrainUniformBuffer uniformBuffer;
		uniformBuffer.terrainTransform = (*it)->getTransform();
		uniformBuffer.scale = (float)(*it)->getScale();
		uniformBuffer.offset = (*it)->getOffset();

		glBindBuffer(GL_UNIFORM_BUFFER, mpTerrainUniformBuffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(TerrainUniformBuffer), &uniformBuffer, GL_DYNAMIC_DRAW);

		mpMesh->Render();
	}

	//mpGrassRenderer->Render(perFrame, perObjectBufferRef, lastProgramReference);
	//std::cout << "Meshes rendered: " << mpQuadTree->mCurrentRenderingNodes.size() << std::endl;

	glUseProgram(lastProgramReference);
}