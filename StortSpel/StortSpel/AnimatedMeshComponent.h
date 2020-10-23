#pragma once
#include "MeshComponent.h"
#include "ConstantBufferTypes.h"
#include "AnimationResource.h"

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

	AnimationResource* m_currentAnimationResource;
	float m_animationTime;
	bool m_shouldLoop;
	bool m_isDone;
	int  m_currentFrame;

public:
	
	AnimatedMeshComponent(const char* filepath, ShaderProgramsEnum shaderEnum = ShaderProgramsEnum::DEFAULT, Material material = Material());
	AnimatedMeshComponent(const char* filepath, Material material);

	skeletonAnimationCBuffer* getAllAnimationTransforms();

	void playAnimation(std::string animationName, bool looping);
	
	void applyAnimationFrame();

	virtual void update(float dt) override;

private:

	joint createJointAndChildren(int currentIndex, int parentIndex, LRSM_JOINT* LRSMJoints);
	void calcInverseBindTransform(joint* thisJoint, Matrix parentBindTransform);
	
	void setAnimatedTransform(joint* thisJoint, ANIMATION_FRAME* animationFrame);

	// Will more than likly need to be retooled after testing is done:
	void applyPoseToJoints(joint* thisJoint, Matrix parentTransform);
	
};

