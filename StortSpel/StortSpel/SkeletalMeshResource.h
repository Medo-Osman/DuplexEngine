#include "3DPCH.h"
#pragma once
#include "MeshResource.h"

struct LRSM_JOINT
{
	int index;
	int nrOfChildren;
	int children[5];

	float translation[3];
	float rotation[4];
};

class SkeletalMeshResource : public MeshResource
{
private:

	int m_rootIndex;
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
	
	int getRootIndex() { return m_rootIndex; }
	void setRootIndex(int rootIndex) { m_rootIndex = rootIndex; }
	
};