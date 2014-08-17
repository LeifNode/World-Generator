#include "LeapControl.h"
#include "GameObjectPlaceholder.h"

using namespace Leap;
using namespace std;

LeapControl::LeapControl()
	:mSphere(NULL)
{
	mController.addListener(*this);
}

LeapControl::~LeapControl()
{
	mController.removeListener(*this);
	delete mSphere;
}

void LeapControl::onInit(const Controller& controller)
{
}

void LeapControl::onConnect(const Controller& controller)
{
	cout << "LEAP connected" << endl;
}

void LeapControl::onDisconnect(const Controller& controller)
{
	cout << "LEAP disconnected" << endl;
}
void LeapControl::onExit(const Controller& controller)
{
}

void LeapControl::onFrame(const Controller& controller)
{
	mCurrentState = LeapControlGroup();
	
	const Frame frame = controller.frame();

	if (!frame.hands().isEmpty())
	{
		int f = 0;

		for (int h = 0; h < frame.hands().count(); h++)
		{
			const FingerList fingers = frame.hands()[h].fingers();

			if (!fingers.isEmpty())
			{
				for (int i = 0; i < fingers.count() && f < 10; i++)
				{
					Vector postition = fingers[i].tipPosition();
					Vector velocity = fingers[i].tipVelocity();

					postition /= 7.0f;
					velocity /= 7.0f;
					postition.z -= 60.0f;
					postition.y -= 30.0f;

					mCurrentState.fingers[f].position = vec4(postition.x, postition.y, postition.z, 1.0f);
					mCurrentState.fingers[f].velocity = vec3(velocity.x, velocity.y, velocity.z);

					if (mCurrentState.fingers[f].velocity.length() > 10.0f)
						mCurrentState.fingers[f].velocity = vec3(0.0f);

					mCurrentState.fingers[f].position = mLastCameraViewInverse * mCurrentState.fingers[f].position;
					mCurrentState.fingers[f].velocity = (mat3)mLastCameraViewInverse * mCurrentState.fingers[f].velocity;

					f++;
				}
			}
		}
	}
}

void LeapControl::onFocusGained(const Controller& controller)
{
}

void LeapControl::onFocusLost(const Controller& controller)
{
}

void LeapControl::Initialize()
{
	mSphere = new GameObjectPlaceHolder(vec3());
	mSphere->mpMaterial = new Material();

	mSphere->mpMaterial->Diffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	mSphere->mpMaterial->Ambient = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	mSphere->mpMaterial->Emissive = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	mSphere->mpMaterial->Specular = vec4(1.0f, 1.0f, 1.0f, 50.0f);

	Mesh sphere;
	GeometryGenerator::GenerateSphere(1.0f, 10, 10, sphere);

	mSphere->addMesh(sphere);
}

void LeapControl::Update(float dt)
{

}

void LeapControl::Render(const PerFrameBlock& perFrame, GLuint bufferRef)
{
	mLastCameraViewInverse = glm::inverse(perFrame.View);

	for (int i = 0; i < 10; i++)
	{
		if (mCurrentState.fingers[i].position.x == 0.0f)
			continue;

		mat4 world = glm::translate(mat4(1.0f), vec3(mCurrentState.fingers[i].position));
		mat4 WVP = perFrame.ViewProj * world;

		PerObjectBlock objectBlock;
		objectBlock.World = world;
		objectBlock.WorldViewProj = WVP;
		objectBlock.WorldInvTranspose = glm::inverse(glm::transpose(world));

		glBindBuffer(GL_UNIFORM_BUFFER, bufferRef);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(PerObjectBlock), &objectBlock, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 2, bufferRef);

		mSphere->Draw(objectBlock, bufferRef);
	}
}