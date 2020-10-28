#include "3DPCH.h"
#pragma once


struct JOINT_TRANSFORM
{
	float translation[3];
	float rotationQuat[4];
	
	DirectX::XMMATRIX asMatrix()
	{
		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::SimpleMath::Quaternion(rotationQuat[0], rotationQuat[1], rotationQuat[2], rotationQuat[3]));
		return XMMatrixMultiply(rotationMatrix, XMMatrixTranslation(translation[0], translation[1], translation[2]));
	}
};

struct ANIMATION_FRAME
{
	float timeStamp;
	JOINT_TRANSFORM* jointTransforms;
};

class AnimationResource
{
private:

	//friend class ResourceHandler;

	float m_timeSpan;
	unsigned int m_frameCount;
	unsigned int m_jointCount;
	ANIMATION_FRAME* m_frames;

public:

	AnimationResource()
		:m_jointCount(0), m_frameCount(0), m_timeSpan(0.f), m_frames(nullptr)
	{}

	~AnimationResource()
	{
		if (m_frames)
		{
			for (size_t i = 0; i < m_frameCount; i++)
			{
				if(m_frames[i].jointTransforms)
					delete[] m_frames[i].jointTransforms;
			}
			delete[] m_frames;
		}
	}

	ANIMATION_FRAME** getFrames() { return &m_frames; }
	float getTimeSpan() { return m_timeSpan; }
	void setTimeSpan(float time) { m_timeSpan = time; }
	
	unsigned int getFrameCount() { return m_frameCount; }
	void setFrameCount(unsigned int  frameCount) { m_frameCount = frameCount; }

	unsigned int getJointCount() { return m_jointCount; }
	void setJointCount(unsigned int jointCount) { m_jointCount = jointCount; }
};