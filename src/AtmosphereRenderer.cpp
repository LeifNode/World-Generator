#include "AtmosphereRenderer.h"
#include "GameObjectPlaceholder.h"
#include "Geometry.h"
#include "LoadShaders.h"

AtmosphereRenderer::AtmosphereRenderer()
	:mpAtmosphereSphere(NULL),
	mpWorldSphere(NULL),
	mpSkyFromAtmosphereProgram(0),
	mpSkyFromSpaceProgram(0),
	mpAtmosphereUniformBuffer(0)
{
}

AtmosphereRenderer::~AtmosphereRenderer()
{
	delete mpAtmosphereSphere;
	mpAtmosphereSphere = nullptr;
	delete mpWorldSphere;
	mpWorldSphere = nullptr;


	glDeleteBuffers(1, &mpAtmosphereUniformBuffer);
	glDeleteProgram(mpSkyFromAtmosphereProgram);
	glDeleteProgram(mpSkyFromSpaceProgram);
}

void AtmosphereRenderer::Initialize()
{
	ShaderInfo fromAtmosphereToSkyInfo[] =
	{
		{ GL_VERTEX_SHADER, "Shaders/atmosphere-sky_vs.glsl" },
		{ GL_FRAGMENT_SHADER, "Shaders/atmosphere-sky_fs.glsl" },
		{ GL_NONE, NULL }
	};

	ShaderInfo fromSpaceToSkyInfo[] =
	{
		{ GL_VERTEX_SHADER, "Shaders/space-sky_vs.glsl" },
		{ GL_FRAGMENT_SHADER, "Shaders/space-sky_fs.glsl" },
		{ GL_NONE, NULL }
	};

	mpSkyFromAtmosphereProgram = LoadShaders(fromAtmosphereToSkyInfo);
	mpSkyFromSpaceProgram = LoadShaders(fromSpaceToSkyInfo);

	GLuint perFrameUniformIndex;
	GLuint perObjectUniformIndex;

	perFrameUniformIndex = glGetUniformBlockIndex(mpSkyFromAtmosphereProgram, "UniformPerFrameBlock");
	glUniformBlockBinding(mpSkyFromAtmosphereProgram, perFrameUniformIndex, 1);

	perObjectUniformIndex = glGetUniformBlockIndex(mpSkyFromAtmosphereProgram, "UniformPerObjectBlock");
	glUniformBlockBinding(mpSkyFromAtmosphereProgram, perObjectUniformIndex, 2);

	perFrameUniformIndex = glGetUniformBlockIndex(mpSkyFromSpaceProgram, "UniformPerFrameBlock");
	glUniformBlockBinding(mpSkyFromSpaceProgram, perFrameUniformIndex, 1);

	perObjectUniformIndex = glGetUniformBlockIndex(mpSkyFromSpaceProgram, "UniformPerObjectBlock");
	glUniformBlockBinding(mpSkyFromSpaceProgram, perObjectUniformIndex, 2);

	glUseProgram(mpSkyFromAtmosphereProgram);

	InitializeAtmosphereUniform();

	mpAtmosphereSphere = new GameObjectPlaceHolder(vec3());
	//mpWorldSphere = new GameObjectPlaceHolder(vec3());

	mpAtmosphereSphere->mpMaterial = new Material();
	//mpWorldSphere->mpMaterial = new Material();

	mpAtmosphereSphere->mpMaterial->Diffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	mpAtmosphereSphere->mpMaterial->Ambient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	mpAtmosphereSphere->mpMaterial->Emissive = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	mpAtmosphereSphere->mpMaterial->Specular = vec4(0.0f, 0.0f, 0.0f, 500.0f);

	//mpWorldSphere->mpMaterial->Diffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//mpWorldSphere->mpMaterial->Ambient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	//mpWorldSphere->mpMaterial->Emissive = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	//mpWorldSphere->mpMaterial->Specular = vec4(0.0f, 0.0f, 0.0f, 50.0f);

	Mesh sphere;

	GeometryGenerator::GenerateSphere(-atmosphereScale, 500, 800, sphere);
	mpAtmosphereSphere->addMesh(sphere);

	//GeometryGenerator::GenerateSphere(worldScale - 50.0f, 100, 800, sphere);
	//mpWorldSphere->addMesh(sphere);
}

void AtmosphereRenderer::InitializeAtmosphereUniform()
{
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

	GLuint perAtmosphereBlockIndex = glGetUniformBlockIndex(mpSkyFromAtmosphereProgram, "UniformAtmosphereBlock");
	glUniformBlockBinding(mpSkyFromAtmosphereProgram, perAtmosphereBlockIndex, 3);

	perAtmosphereBlockIndex = glGetUniformBlockIndex(mpSkyFromSpaceProgram, "UniformAtmosphereBlock");
	glUniformBlockBinding(mpSkyFromSpaceProgram, perAtmosphereBlockIndex, 3);

	glBindBuffer(GL_UNIFORM_BUFFER, mpAtmosphereUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PerAtmosphereBlock), &atmosphereBlock, GL_STATIC_COPY);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, mpAtmosphereUniformBuffer);
}

void AtmosphereRenderer::Render(const PerFrameBlock& perFrame, GLuint perObjectBufferRef, GLuint lastProgramReference)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	PerObjectBlock objectBlock;

	mat4 atmosphereWorld = glm::translate(mat4(1.0f), vec3(0.0f, -worldScale, 0.0f));

	mat4 atmosphereMVP = perFrame.ViewProj * atmosphereWorld;

	objectBlock.World = atmosphereWorld;
	objectBlock.WorldViewProj = atmosphereMVP;
	objectBlock.WorldInvTranspose = glm::inverse(glm::transpose(atmosphereWorld));

	//mpWorldSphere->Draw(objectBlock, perObjectBufferRef);

	if (glm::length(perFrame.ViewPosition + vec3(0.0f, worldScale, 0.0f)) < atmosphereScale)
		glUseProgram(mpSkyFromAtmosphereProgram);
	else
		glUseProgram(mpSkyFromSpaceProgram);

	glBindBufferBase(GL_UNIFORM_BUFFER, 3, mpAtmosphereUniformBuffer);


	//glDisable(GL_DEPTH_TEST);
	//glDepthMask(false);

	mpAtmosphereSphere->Draw(objectBlock, perObjectBufferRef);

	//glDepthMask(true);
	//glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glUseProgram(lastProgramReference);
}