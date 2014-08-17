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