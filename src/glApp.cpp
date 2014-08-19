#include "glApp.h"
#include "LoadShaders.h"
#include "Geometry.h"
#include <sstream>
#include "COLLADALoader.h"
#include "UniformBlocks.h"
#include "TextureManager.h"
#include "ParticleSystem.h"
#include "LeapControl.h"
#include "AtmosphereRenderer.h"
#include "SkyboxRenderer.h"
#include "TerrainRenderer.h"
#include "VoxelRenderer.h"
#include "OceanPlane.h"

GLApp::GLApp(GLFWwindow* window)
	:pWindow(window),
	mCurrentRotation(0.0f),
	mClientWidth(1280.0f),
	mClientHeight(720.0f),
	mCamera(window, vec3(-20.0f, 10.0f, -20.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)),
	mWireFrame(false),
	mCullBackfaces(false),
	mRotateModels(true),
	mpParticleSystem(NULL),
	mpAtmosphereRenderer(NULL),
	mpTerrain(NULL),
	mSunDirection(0.0f, 1.0f, 0.0f)
{
	mpParticleSystem = new ParticleSystem();
	mpAtmosphereRenderer = new AtmosphereRenderer();
	mpSkybox = new SkyboxRenderer();
	mpTerrain = new TerrainRenderer(worldScale);
	mpVoxels = new VoxelRenderer();
	mpOcean = new OceanPlane(&mCamera);
}

GLApp::~GLApp()
{
	delete mpParticleSystem;
	delete mpAtmosphereRenderer;
	delete mpSkybox;
	delete mpVoxels;

	mpParticleSystem = nullptr;
	mpAtmosphereRenderer = nullptr;
	mpSkybox = nullptr;
	mpVoxels = nullptr;

	for (auto it = mLoadedModels.begin(); it != mLoadedModels.end(); ++it)
		delete (*it);

	std::cout << "Deleted" << std::endl;

	glUseProgram(0);
}

void GLApp::Initialize()
{
	mTimer.Reset();
	mTimer.Start();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_CULL_FACE);

	//Loading, Compiling, and Linking shaders
	ShaderInfo shaderInfo[] = 
	{
		{ GL_VERTEX_SHADER, "Shaders/basic_vs.glsl" },
		{ GL_FRAGMENT_SHADER, "Shaders/basic_fs.glsl" },
		{ GL_NONE, NULL}
	};

	renderProgram = LoadShaders(shaderInfo);
	glUseProgram(renderProgram);

	glGenBuffers(1, &perFrameBuffer);
	glGenBuffers(1, &perObjectBuffer);

	GLuint perFrameUniformIndex, perObjectUniformIndex;

	perObjectUniformIndex = glGetUniformBlockIndex(renderProgram, "UniformPerObjectBlock");
	perFrameUniformIndex = glGetUniformBlockIndex(renderProgram, "UniformPerFrameBlock");

	glUniformBlockBinding(renderProgram, perFrameUniformIndex, 1);
	glUniformBlockBinding(renderProgram, perObjectUniformIndex, 2);

	PerFrameBlock perFrame;

	glBindBuffer(GL_UNIFORM_BUFFER, perFrameBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PerFrameBlock), &perFrame, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, perFrameBuffer);

	PerObjectBlock objectBlock;

	glBindBuffer(GL_UNIFORM_BUFFER, perObjectBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PerObjectBlock), &objectBlock, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, perObjectBuffer);

	glUniform1i(glGetUniformLocation(renderProgram, "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(renderProgram, "bumpMap"), 1);
	glUniform1i(glGetUniformLocation(renderProgram, "specularMap"), 2);
	glUniform1i(glGetUniformLocation(renderProgram, "emissiveMap"), 3);

	TextureManager::getInstance().loadTexture("rock.dds");
	TextureManager::getInstance().loadTexture("rock_n.dds");

	Mesh plane;
	GeometryGenerator::GeneratePlane(1000.0f, 1000.0f, 256, 256, plane);

	GameObjectPlaceHolder* ground = new GameObjectPlaceHolder(vec3(0.0f, 0.0f, 0.0f));
	mLoadedModels.push_back(ground);

	ground->addMesh(plane);
	ground->mpMaterial = new Material();
	ground->mpMaterial->Diffuse = vec4(1.0f);
	ground->mpMaterial->Ambient = vec4(1.0f);
	ground->mpMaterial->Specular = vec4(0.4f);
	ground->mpMaterial->Specular.w = 80.0f;

	ground->mTextureTransform = glm::scale(mat4(1.0f), vec3(10.0f, 0.0f, 10.0f));

	ground->mpDiffuseTexture = TextureManager::getInstance().getTexture("rock.dds");
	ground->mpBumpMap = TextureManager::getInstance().getTexture("rock_n.dds");

	LeapControl::getInstance().Initialize();

	//mpParticleSystem->Initialize();
	mpAtmosphereRenderer->Initialize();
	mpSkybox->Initialize();
	mpTerrain->Initialize();
	//mpVoxels->Initialize();
	//mpOcean->Initialize();
}

void GLApp::InitializeModels(int argc, char* argv[])
{
	TextureManager& textureManager = TextureManager::getInstance();

	for (int i = 1; i < argc; i++)
	{
		COLLADALoader loader(argv[i]);
		loader.loadDocument();

		loader.parse();

		collada::SceneNode* currentNode = loader.getRootNode();

		GameObjectPlaceHolder* rootObject = new GameObjectPlaceHolder(vec3(3.5f * (i - 1), 3.0f, 0.0f));
		mLoadedModels.push_back(rootObject);

		InitializeFromColladaNode(loader, rootObject, currentNode);
	}
}

void GLApp::InitializeFromColladaNode(COLLADALoader& loader, GameObjectPlaceHolder* gameObjectNode, collada::SceneNode* colladaNode)
{
	TextureManager& textureManager = TextureManager::getInstance();

	if (colladaNode->effectId != "")
	{
		const collada::Effect* effect = loader.getEffect(colladaNode->effectId);

		Material* material = new Material();
		gameObjectNode->mpMaterial = material;

		material->Ambient = effect->ambient;
		material->Diffuse = effect->diffuse;
		material->Specular = effect->specular;
		material->Specular.w = effect->specularPower; //Pack specular power into w
		material->Emissive = effect->emission;

		if (effect->diffuseTextureId != "")
		{
			textureManager.loadTexture(effect->diffuseTextureId.c_str());
			gameObjectNode->mpDiffuseTexture = textureManager.getTexture(effect->diffuseTextureId.c_str());
		}
		if (effect->bumpMapId != "")
		{
			textureManager.loadTexture(effect->bumpMapId.c_str());
			gameObjectNode->mpBumpMap = textureManager.getTexture(effect->bumpMapId.c_str());
		}
		if (effect->specularMapId != "")
		{
			textureManager.loadTexture(effect->specularMapId.c_str());
			gameObjectNode->mpSpecularMap = textureManager.getTexture(effect->specularMapId.c_str());
		}
		if (effect->emissiveMapId != "")
		{
			textureManager.loadTexture(effect->emissiveMapId.c_str());
			gameObjectNode->mpEmissiveMap = textureManager.getTexture(effect->emissiveMapId.c_str());
		}
	}

	//Load submeshes
	if (colladaNode->model)
	{
		for (auto it = colladaNode->model->subMeshes.begin(); it != colladaNode->model->subMeshes.end(); ++it)
		{
			gameObjectNode->addMesh((*it)->mesh);

			RenderableMesh* currentMesh = *gameObjectNode->mMeshes.rbegin();

			if ((*it)->effectId != "")
			{
				const collada::Effect* effect = loader.getEffect((*it)->effectId);

				Material* material = new Material();
				gameObjectNode->mpMaterial = material;

				material->Ambient = effect->ambient;
				material->Diffuse = effect->diffuse;
				material->Specular = effect->specular;
				material->Specular.w = effect->specularPower; //Pack specular power into w
				material->Emissive = effect->emission;

				if (effect->diffuseTextureId != "")
				{
					textureManager.loadTexture(effect->diffuseTextureId.c_str());
					currentMesh->mpDiffuseTexture = textureManager.getTexture(effect->diffuseTextureId.c_str());
				}
				if (effect->bumpMapId != "")
				{
					textureManager.loadTexture(effect->bumpMapId.c_str());
					currentMesh->mpBumpMap = textureManager.getTexture(effect->bumpMapId.c_str());
				}
				if (effect->specularMapId != "")
				{
					textureManager.loadTexture(effect->specularMapId.c_str());
					currentMesh->mpSpecularMap = textureManager.getTexture(effect->specularMapId.c_str());
				}
				if (effect->emissiveMapId != "")
				{
					textureManager.loadTexture(effect->emissiveMapId.c_str());
					currentMesh->mpEmissiveMap = textureManager.getTexture(effect->emissiveMapId.c_str());
				}
			}
		}
	}

	//Go recursively through children
	for (auto it = colladaNode->children.begin(); it != colladaNode->children.end(); ++it)
	{
		GameObjectPlaceHolder* newGameObject = new GameObjectPlaceHolder(vec3(0.0f));
		gameObjectNode->mChildren.push_back(newGameObject);

		InitializeFromColladaNode(loader, newGameObject, (*it));
	}
}

void GLApp::OnResize(int width, int height)
{
	mClientWidth = (float)width;
	mClientHeight = (float)height;

	glViewport(0, 0, width, height);
	mCamera.OnScreenResize(width, height);
}

void GLApp::OnKey(int key, int action)
{
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		mWireFrame = !mWireFrame;

		if (mWireFrame)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		mCullBackfaces = !mCullBackfaces;

		if (mCullBackfaces)
			glDisable(GL_CULL_FACE);
		else
			glEnable(GL_CULL_FACE);
	}
	else if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		mRotateModels = !mRotateModels;
	}
}

void GLApp::OnMouseMove(float x, float y)
{
	mCamera.OnMouseMove(x, y);
}

void GLApp::OnMouseDown(int button)
{
	glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	mCamera.OnMouseDown(button);
}

void GLApp::OnMouseUp(int button)
{
	glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	mCamera.OnMouseUp(button);
}

void GLApp::Update()
{
	mTimer.Tick();

	if (mRotateModels)
		mCurrentRotation += (float)mTimer.DeltaTime() * (360.0f / 15.0f);

	static const float moveSpeed = 100000.0f;

	mCamera.Update((float)mTimer.DeltaTime());

	if (glfwGetKey(pWindow, GLFW_KEY_Z) == GLFW_PRESS)
	{
		mSunDirection = glm::normalize((mat3)glm::rotate(mat4(1.0f), (float)(10.1f * mTimer.DeltaTime()), glm::normalize(vec3(1.0f, 0.2f, 0.0f))) * mSunDirection);
	}
	if (glfwGetKey(pWindow, GLFW_KEY_X) == GLFW_PRESS)
	{
		mSunDirection = glm::normalize((mat3)glm::rotate(mat4(1.0f), -(float)(10.1f * mTimer.DeltaTime()), glm::normalize(vec3(1.0f, 0.2f, 0.0f))) * mSunDirection);
	}

	UpdateFrameCounter(mTimer.DeltaTime());

	mpTerrain->Update(mCamera, (float)mTimer.DeltaTime());
	//mpParticleSystem->Update((float)mTimer.DeltaTime());
}

void GLApp::UpdateFrameCounter(double dt)
{
	static double totalTime = 0.0f;
	static int timesHit = 0;

	if (totalTime >= 1.0f)
	{
		double averageime = totalTime / (float)timesHit;
		totalTime = 0.0f;
		timesHit = 0;

		std::stringstream title;
		title << "Frame time: " << averageime * 1000.0f << "ms   FPS: " << 1.0f / averageime << " Velocity: " << mCamera.getVelocity() << "m/s";
		glfwSetWindowTitle(pWindow, title.str().c_str());
	}
	else
	{
		totalTime += mTimer.DeltaTime();
		timesHit++;
	}
}

void GLApp::PreRender()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void GLApp::Render()
{
    glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glUseProgram(renderProgram);

	DirectionalLight sun;
	sun.Diffuse = vec4(0.4f, 0.4f, 0.4f, 1.0f);
	sun.Direction = mSunDirection;
	sun.Ambient = vec4(0.03f, 0.03f, 0.03f, 1.0f);

	PointLight point;
	point.Position = vec3(0.0f, 10.0f, 5.0f);
	point.Ambient = vec4(0.04f, 0.0f, 0.0f, 1.0f);
	point.Diffuse = vec4(0.18f, 0.0f, 0.0f, 1.0f);
	point.Range = 1000.0f;
	point.Att = vec3(0.0f, 0.1f, 0.001f);

	SpotLight spotLight;
	spotLight.Ambient = vec4(0.0f, 0.01f, 0.0f, 1.0f);
	spotLight.Diffuse = vec4(0.0, 0.20f, 0.0f, 1.0f);
	spotLight.Position = vec3(-60.0f, 60.0f, 60.0f);
	spotLight.Direction = glm::normalize(vec3(0.0f, 0.0f, 0.0f) - spotLight.Position);
	spotLight.Att = vec3(0.0f, 0.01f, 0.0001f);
	spotLight.Range =10000.0f;
	spotLight.Spot = 200.1f;

	PerFrameBlock perFrame;
	perFrame.View = mCamera.getView();
	perFrame.Proj = mCamera.getProj();
	perFrame.ViewInvTrans = glm::inverse(glm::transpose(mCamera.getView()));
	perFrame.ViewPosition = mCamera.position;
	perFrame.ViewPositionD = mCamera.position;
	perFrame.ViewProj = mCamera.getProj() * mCamera.getView();
	perFrame.nearClippingPlane = mCamera.getNear();
	perFrame.farClippingPlane = mCamera.getFar();
	perFrame.directionalLight = sun;
	perFrame.pointLight = point;
	perFrame.spotLight = spotLight;

	glBindBuffer(GL_UNIFORM_BUFFER, perFrameBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PerFrameBlock), &perFrame, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, perFrameBuffer);

	mat4 projection = mCamera.getProj();
	mat4 view = mCamera.getView();
	mat4 world = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));

	mpSkybox->Render(perFrame, perObjectBuffer, renderProgram);

	//mpVoxels->Render(perFrame, perObjectBuffer, renderProgram);

	for (auto it = mLoadedModels.begin(); it != mLoadedModels.end(); ++it)
	{
		Material material;

		material.Diffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		material.Ambient = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		material.Emissive = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		material.Specular = vec4(1.0f, 1.0f, 1.0f, 50.0f);

		if (it != mLoadedModels.begin())
			world = glm::translate(mat4(1.0f), (*it)->mPosition) * glm::scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f)) * glm::rotate(mat4(1.0f), mCurrentRotation, vec3(0.0f, 1.0f, 0.0f));
	
		mat4 WVP = projection * view * world;

		PerObjectBlock objectBlock;
		objectBlock.ObjectMaterial = material;
		objectBlock.World = world;
		objectBlock.WorldViewProj = WVP;
		objectBlock.WorldInvTranspose = glm::inverse(glm::transpose(world));

		glBindBuffer(GL_UNIFORM_BUFFER, perObjectBuffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(PerObjectBlock), &objectBlock, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 2, perObjectBuffer);

		(*it)->Draw(objectBlock, perObjectBuffer);
	}

	mpTerrain->Render(perFrame, perObjectBuffer, renderProgram);
	mpAtmosphereRenderer->Render(perFrame, perObjectBuffer, renderProgram);

	LeapControl::getInstance().Render(perFrame, perObjectBuffer);

	//mpParticleSystem->Draw();
	//mpOcean->Render(perFrame, perObjectBuffer, renderProgram);
}

void GLApp::PostRender()
{

}