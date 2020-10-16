#include "3DPCH.h"
#pragma once
#include "MeshResource.h"

struct LRSM_JOINT
{
	int index;
	int nrOfChildren;
	int children[5];

	float translation[4]; //TODO: make these float3s, and do the same in the converter to save file space.
	float rotation[4];

	float tempPoseRotation[4];
	//DirectX::XMVECTOR translation;
	//DirectX::XMVECTOR rotation;
};

class SkeletalMeshResource : public MeshResource
{
private:

	int m_jointCount;
	LRSM_JOINT* m_inputJoints = nullptr;

public:

	~SkeletalMeshResource()
	{
		if (m_inputJoints != nullptr)
			delete[] m_inputJoints;
	}

	LRSM_JOINT* getJoints() { return m_inputJoints; }
	void setJoints(LRSM_JOINT* joints) { m_inputJoints = joints; }
	
	int getJointCount() { return m_jointCount; }
	void setJointCount(int jointCount) { m_jointCount = jointCount; }
	
};