#pragma once
#include "MeshComponent.h"
#include "ConstantBufferTypes.h"

struct joint
{
	int index;
	std::vector<joint> children;

	Matrix animatedTransform;

	Matrix localBindTransform;
	Matrix inverseBindTransform;
};

class AnimatedMeshComponent : public MeshComponent
{
private:

	int m_jointCount;
	joint m_rootJoint;
	skeletonAnimationCBuffer m_cBufferStruct;

public:
	
	AnimatedMeshComponent(const char* filepath, ShaderProgramsEnum shaderEnum = ShaderProgramsEnum::DEFAULT, Material material = Material());
	AnimatedMeshComponent(const char* filepath, Material material);

	skeletonAnimationCBuffer* getAllAnimationTransforms();

private:

	joint createJointAndChildren(int currentIndex, int parentIndex, LRSM_JOINT* LRSMJoints);
	void calcInverseBindTransform(joint* thisJoint, Matrix parentBindTransform);
	
	// Will more than likly need to be retooled after testing is done:
	void applyPoseToJoints(joint* thisJoint, Matrix parentTransform);
	
};

