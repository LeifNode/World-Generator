#pragma once

#include "glStd.h"
#include "GameTimer.h"
#include "GameObjectPlaceholder.h"
#include "Camera.h"
#include "COLLADALoader.h"

class ParticleSystem;
class AtmosphereRenderer;
class SkyboxRenderer;
class TerrainRenderer;
class VoxelRenderer;
class OceanPlane;

class GLApp
{
public:
	GLApp(GLFWwindow* window);
	~GLApp();

	void Initialize();
	void InitializeModels(int argc, char* argv[]);
	void InitializeFromColladaNode(COLLADALoader& loader, GameObjectPlaceHolder* gameObjectNode, collada::SceneNode* colladaNode);

	void OnResize(int width, int height);

	void OnKey(int key, int action);
	void OnMouseMove(float x, float y);
	void OnMouseDown(int button);
	void OnMouseUp(int button);

	void Update();

	void PreRender();
	void Render();
	void PostRender();

private:
	void UpdateFrameCounter(double dt);

private:
	GameTimer mTimer;

	GLFWwindow* pWindow;

	GLuint renderProgram;

	GLuint perFrameBuffer;
	GLuint perObjectBuffer;

	GLint mShaderWorldTransformLoc;
	GLint mShaderViewLoc;
	GLint mShaderProjectionLoc;

	std::vector<GameObjectPlaceHolder*> mLoadedModels; 

	Camera mCamera;

	ParticleSystem* mpParticleSystem;
	AtmosphereRenderer* mpAtmosphereRenderer;
	SkyboxRenderer* mpSkybox;
	TerrainRenderer* mpTerrain;
	VoxelRenderer* mpVoxels;
	OceanPlane* mpOcean;

	vec3 mSunDirection;

	float mClientWidth;
	float mClientHeight;

	float mCurrentRotation;

	bool mWireFrame;
	bool mCullBackfaces;
	bool mRotateModels;
};