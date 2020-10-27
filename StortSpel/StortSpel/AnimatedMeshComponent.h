#pragma once
#include "MeshComponent.h"
#include "ConstantBufferTypes.h"
#include "AnimationResource.h"

struct joint
{
	int index;
	//std::vector<joint> children;
	std::vector<int> children;

	Matrix animatedTransform;

	Matrix localBindTransform;
	Matrix inverseBindTransform;
};

class AnimatedMeshComponent : public MeshComponent
{
private:

	int m_jointCount;
	int m_rootIdx;
	//joint m_rootJoint;
	std::vector<joint> m_joints;
	skeletonAnimationCBuffer m_cBufferStruct;

	AnimationResource* m_currentAnimationResource;
	float m_animationTime;
	bool m_shouldLoop;
	bool m_isDone;
	float m_animationSpeed;

public:
	
	AnimatedMeshComponent(const char* filepath, ShaderProgramsEnum shaderEnum = ShaderProgramsEnum::DEFAULT, Material material = Material());
	AnimatedMeshComponent(const char* filepath, Material material);

	skeletonAnimationCBuffer* getAllAnimationTransforms();

	void playAnimation(std::string animationName, bool looping);
	
	void applyAnimationFrame();

	virtual void update(float dt) override;

	// Deltatime is multiplied by this number when the component updates.
	void setAnimationSpeed(const float newAnimationSpeed) { m_animationSpeed = newAnimationSpeed; }

private:

	joint createJointAndChildren(int currentIndex, LRSM_JOINT* LRSMJoints);
	void calcInverseBindTransform(int thisJointIdx, Matrix parentBindTransform);
	
	// Updates the animatedTransform of all the joints using a ANIMATION_FRAME struct.
	void setAnimatedTransform(int thisJointIdx, ANIMATION_FRAME* animationFrame);

	// Applies the correct matrices to the cbuffer, should only be called if all joint's animatedTransform is updated
	void applyPoseToJoints(int thisJointIdx, Matrix parentTransform);
	
};

