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

using glm::quat;

class Camera
{
public:
	//Camera();
	Camera(GLFWwindow* window, const vec3& position, const vec3& direction, const vec3& up);
	~Camera();

	mat4 getView();
	mat4 getProj() const { return mProjection; }

	__declspec(property(get = getPosition, put = setPosition)) dvec3 position;
	dvec3 getPosition() const { return mPosition; }
	void setPosition(const dvec3& newPosition) { mPosition = newPosition; }

	__declspec(property(get = getDirection, put = setDirection)) vec3 direction;
	vec3 getDirection();
	void setDirection(const vec3& newDir);

	__declspec(property(get = getPitch, put = setPitch)) float pitch;
	float getPitch() const { return mPitch; }
	void setPitch(float pitch);

	__declspec(property(get = getYaw, put = setYaw)) float yaw;
	float getYaw() const { return mYaw; }
	void setYaw(float yaw) { mYaw = yaw; mDirectionChanged = true; }

	float getNear() const { return mNear; }
	float getFar() const { return mFar; }

	float getVelocity() const { return mVelocity; }

	void OnMouseMove(float x, float y);
	void OnMouseDown(int button);
	void OnMouseUp(int button);
	void OnScreenResize(int width, int height);

	void Update(float dt);

private:
	void updatePosition(float dt);
	void updateOrientation(float dt);

	void setProjection(int width, int height, float fovVertical);
	mat4 getLookAt(const vec3& eye, const vec3& direction, const vec3& up);

private:
	GLFWwindow* mpWindow;

	mat4 mProjection;

	dvec3 mPosition;
	vec3 mDirection;
	vec3 mAdjustedDirection;
	vec3 mUp, mRight;
	quat mRotation;

	float mNear;
	float mFar;
	float mVelocity;

	float mPitch, mYaw;
	vec2 mLastMousePosition;

	bool mReadingMouse;
	bool mDirectionChanged;
	bool mFreeRoam;
};