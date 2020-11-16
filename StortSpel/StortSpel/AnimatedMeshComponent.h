#pragma once
#include "MeshComponent.h"
#include "ConstantBufferTypes.h"
#include "AnimationResource.h"



struct joint
{
	int index;

	std::vector<int> children;

	Matrix animatedTransform;

	Matrix inverseBindTransform;
};

struct animationStruct
{
	AnimationResource* animationResource;

	float animationTime;

	float animationSpeed;

	std::string animationName;
};

struct animState
{
	std::vector<animationStruct> structs;
	
	bool justOne;

	std::vector<float> blendPoints;
	
	float blend;

	float startTransitionDuration;
	bool playDuringStartTransistion;

	std::string stateName;
};

class AnimatedMeshComponent : public MeshComponent
{
private:
	int m_jointCount;
	int m_rootIdx;
	std::vector<joint> m_joints;
	skeletonAnimationCBuffer m_cBufferStruct;

	bool m_inBindPose;
	bool m_justOnePose;
	std::unordered_map<std::string, animState> m_storedStates;
	animState* m_currentState;
	std::queue<animState*> m_animationQueue;
	
	// when m_transitionTime is 0 no transtion is happening, when over 0 it means that it is counting down
	float m_transitionTime;

public:
	
	AnimatedMeshComponent(const char* filepath, std::initializer_list<ShaderProgramsEnum> shaderEnums = { ShaderProgramsEnum::DEFAULT }, std::initializer_list<Material> materials = { Material() });
	AnimatedMeshComponent(const char* filepath, ShaderProgramsEnum shaderEnum, std::initializer_list<Material> materials = { Material() });
	AnimatedMeshComponent(const char* filepath, ShaderProgramsEnum shaderEnum, Material material);
	AnimatedMeshComponent(const char* filepath, Material material);
	AnimatedMeshComponent(const char* filepath, std::initializer_list<Material> materials);

	skeletonAnimationCBuffer* getAllAnimationTransforms();

	void playSingleAnimation(std::string animationName, float transistionTime, bool playDuringStartTransistion);
	void addSingleAnimation(std::string animationName, float transistionTime, bool playDuringStartTransistion);
	void queueSingleAnimation(std::string animationName, float transistionTime, bool playDuringStartTransistion);

	void addBlendState(const std::initializer_list<std::pair<const std::string, float>>& animationParams, std::string stateName, bool playDuringStartTransistion);
	void addAndPlayBlendState(const std::initializer_list<std::pair<const std::string, float>>& animationParams, std::string stateName, float transistionTime, bool playDuringStartTransistion);
	bool playBlendState(std::string stateName, float transistionTime);
	bool queueBlendState(std::string stateName, float transistionTime);
	
	void setCurrentBlend(float blend);
	float getCurrentBlend();

	virtual void update(float dt) override;

	// Deltatime is multiplied by this number when the component updates.
	void setAnimationSpeed(const float newAnimationSpeed);
	void setAnimationSpeed(const unsigned int structIndex, const float newAnimationSpeed);

	//std::string getAnimationName();

private:

	void applyAnimationFrame();

	joint createJointAndChildren(int currentIndex, LRSM_JOINT* LRSMJoints, Matrix parentBindTransform);
	
	// Updates the animatedTransform of all the joints using a ANIMATION_FRAME struct.
	void setAnimatedTransform(ANIMATION_FRAME* animationFrame);

	// Applies the correct matrices to the cbuffer, should only be called if all joint's animatedTransform is updated
	void applyPoseToJoints(int thisJointIdx, Matrix parentTransform);
	
	// Takes in an animState and calulates its current ANIMATION_FRAME
	void calculateFrameForState(animState* state, ANIMATION_FRAME** animStateFrame);

	// Produces an ANIMATION_FRAME that is an interpolation of two others
	void interpolateFrame(ANIMATION_FRAME* prevFrame, ANIMATION_FRAME* nextFrame, float progression, ANIMATION_FRAME* interpolatedFrame);

	void advanceQueue();
};

