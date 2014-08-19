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