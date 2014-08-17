#pragma once

#include "glStd.h"
#include <leap\Leap.h>
#include "UniformBlocks.h"

struct FingerPoint
{
	vec4 position;
	vec3 velocity;
	float pad;
};

struct LeapControlGroup
{
	LeapControlGroup() 
	{
		ZeroMemory(this, sizeof(this));
	}

	FingerPoint fingers[10];
};

class GameObjectPlaceHolder;

class LeapControl : Leap::Listener
{
public:
	static LeapControl& getInstance()
    {
        static LeapControl instance;

        return instance;
    }

	virtual void onInit(const Leap::Controller&);
	virtual void onConnect(const Leap::Controller&);
    virtual void onDisconnect(const Leap::Controller&);
    virtual void onExit(const Leap::Controller&);
    virtual void onFrame(const Leap::Controller&);
    virtual void onFocusGained(const Leap::Controller&);
    virtual void onFocusLost(const Leap::Controller&);

	void Initialize();

	void Update(float dt);
	void Render(const PerFrameBlock& perFrame, GLuint bufferRef);

	LeapControlGroup getState() const { return mCurrentState; }

private:
	LeapControl();
	~LeapControl();

	mat4 mLastCameraViewInverse;

	LeapControlGroup mCurrentState;

	GameObjectPlaceHolder* mSphere;

	Leap::Controller mController;
};