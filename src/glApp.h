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