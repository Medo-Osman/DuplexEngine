#include "3DPCH.h"
#include "AnimatedMeshComponent.h"

void AnimatedMeshComponent::init(const char* filepath)
{
	m_inBindPose = true;
	m_transitionTime = 0.f;
	m_type = ComponentType::ANIM_MESH;
	m_filePath = filepath;

	SkeletalMeshResource* resPtr = dynamic_cast<SkeletalMeshResource*>(ResourceHandler::get().loadLRSMMesh(filepath));

	setMeshResourcePtr(resPtr);

	//take the joints from the meshresource and build the joints
	m_jointCount = resPtr->getJointCount();

	m_joints.reserve(m_jointCount);
	for (int i = 0; i < m_jointCount; i++)
	{
		m_joints.push_back(joint());
	}

	m_rootIdx = resPtr->getRootIndex();
	m_joints.at(m_rootIdx) = createJointAndChildren(m_rootIdx, resPtr->getJoints(), XMMatrixIdentity());

	for (int i = 0; i < m_jointCount; i++)
	{
		m_cBufferStruct.boneMatrixPallet[i] = XMMatrixIdentity();
	}

	getMeshResourcePtr()->addRef();
	// when the temp rotation values go this might not need to be here
	//applyPoseToJoints(XMMatrixIdentity());
}

AnimatedMeshComponent::AnimatedMeshComponent(const char* filepath, std::initializer_list<ShaderProgramsEnum> shaderEnums, std::initializer_list<Material> materials)
	:MeshComponent(shaderEnums, materials)
{
	init(filepath);
}

AnimatedMeshComponent::AnimatedMeshComponent(const char* filepath, ShaderProgramsEnum shaderEnum, std::initializer_list<Material> materials)
	:AnimatedMeshComponent(filepath, { shaderEnum }, materials)
{}

AnimatedMeshComponent::AnimatedMeshComponent(const char* filepath, ShaderProgramsEnum shaderEnum, Material material)
	: AnimatedMeshComponent(filepath, { shaderEnum }, { material })
{}

AnimatedMeshComponent::AnimatedMeshComponent(const char* filepath, Material material)
	: AnimatedMeshComponent(filepath, ShaderProgramsEnum::SKEL_ANIM, material)
{}

AnimatedMeshComponent::AnimatedMeshComponent(const char* filepath, std::initializer_list<Material> materials)
	: AnimatedMeshComponent(filepath, ShaderProgramsEnum::SKEL_ANIM, materials)
{}

AnimatedMeshComponent::AnimatedMeshComponent(char* paramData)
	:MeshComponent()
{
	int offset = 0;
	MeshComponent::init({ ShaderProgramsEnum::SKEL_ANIM }, { Material() });
	init(readStringFromChar(paramData, offset).c_str());
}

//std::string AnimatedMeshComponent::getAnimationName()
//{
//	return m_animationName;
//}

joint AnimatedMeshComponent::createJointAndChildren(int currentIndex, LRSM_JOINT* LRSMJoint, Matrix parentBindTransform)
{
	joint thisJoint;

	// Set the joint's index
	thisJoint.index = currentIndex;
	
	// Calculate the localBindTransform using
	XMMATRIX localBindTransform =
		XMMatrixRotationX(XMConvertToRadians(LRSMJoint[currentIndex].rotation[0])) 
		*
		XMMatrixRotationY(XMConvertToRadians(LRSMJoint[currentIndex].rotation[1])) 
		*
		XMMatrixRotationZ(XMConvertToRadians(LRSMJoint[currentIndex].rotation[2]))
		;

	localBindTransform = XMMatrixMultiply(
		localBindTransform,
		XMMatrixTranslation(LRSMJoint[currentIndex].translation[0], LRSMJoint[currentIndex].translation[1], LRSMJoint[currentIndex].translation[2]) 
	);
	/* 
	// old code from when the .lrsm included a single pose aside from the bindpose
	Quaternion tempPoseRotationQuat = Quaternion(LRSMJoint[currentIndex].tempPoseRotation[0], LRSMJoint[currentIndex].tempPoseRotation[1], LRSMJoint[currentIndex].tempPoseRotation[2], LRSMJoint[currentIndex].tempPoseRotation[3]);
	
	thisJoint.animatedTransform = XMMatrixMultiply(
		XMMatrixRotationQuaternion(tempPoseRotationQuat)
		,
		XMMatrixTranslation(LRSMJoint[currentIndex].translation[0], LRSMJoint[currentIndex].translation[1], LRSMJoint[currentIndex].translation[2]) 
	);
	*/
	
	//thisJoint.animatedTransform = localBindTransform;

	Matrix bindTransform = XMMatrixMultiply(localBindTransform, parentBindTransform);

	thisJoint.inverseBindTransform = bindTransform.Invert();

	for (int i = 0; i < LRSMJoint[currentIndex].nrOfChildren; i++)
	{
		thisJoint.children.push_back(LRSMJoint[currentIndex].children[i]);
		m_joints.at(LRSMJoint[currentIndex].children[i]) = createJointAndChildren(LRSMJoint[currentIndex].children[i], LRSMJoint, bindTransform);
	}

	return thisJoint;
}

void AnimatedMeshComponent::setAnimatedTransform(ANIMATION_FRAME* animationFrame)
{
	for (int i = 0; i < m_jointCount; i++)
	{
		m_joints[i].animatedTransform = animationFrame->jointTransforms[i].asMatrix();
	}
}

void AnimatedMeshComponent::applyPoseToJoints(int thisJointIdx, Matrix parentTransform)
{
	Matrix currentTransform = XMMatrixMultiply(m_joints[thisJointIdx].animatedTransform, parentTransform );
	
	//Vector4 testVec = XMVector3Transform(Vector3(0, 0, 0), currentTransform);

	for (int i = 0; i < m_joints[thisJointIdx].children.size(); i++)
	{
		applyPoseToJoints(m_joints[thisJointIdx].children.at(i), currentTransform);
	}

	m_cBufferStruct.boneMatrixPallet[thisJointIdx] = XMMatrixTranspose( XMMatrixMultiply(m_joints[thisJointIdx].inverseBindTransform, currentTransform ) );
}

skeletonAnimationCBuffer* AnimatedMeshComponent::getAllAnimationTransforms()
{
	return &m_cBufferStruct;
}


void AnimatedMeshComponent::playSingleAnimation(std::string animationName, float transistionTime, bool playDuringStartTransistion, bool playDuringEndTransistion)
{
	if (!m_inBindPose)
	{
		if (m_transitionTime > 0.f || transistionTime > 0.f)
        {
            if (!m_animationQueue.empty() && m_animationQueue.front()->stateName == animationName)
                return;
        }
        else
        {
            if (m_currentState->justOne && m_currentState->structs.at(0).animationName == animationName)
                return;
        }
	}

	std::queue<animState*> empty = std::queue<animState*>(); // Make the queue empty by swaping it with an empty one
	std::swap(m_animationQueue, empty);

	if (m_storedStates.find(animationName) == m_storedStates.end()) // If the animation isn't in the stored states map
	{
		addSingleAnimation(animationName, transistionTime, playDuringStartTransistion, playDuringEndTransistion);
	}
	else
	{
		m_storedStates[animationName].startTransitionDuration = transistionTime; // the state is stored alrady so we'll use it and just set the transistionTime
		if (!m_storedStates[animationName].playDuringStartTransistion)
		{
			for (auto& animStruct : m_storedStates[animationName].structs)
			{
				animStruct.animationTime = 0.f;
			}
		}
	}
		
	if (!m_inBindPose && transistionTime > 0.f)
	{
		m_animationQueue.push(&m_storedStates[animationName]);
		m_transitionTime = transistionTime;
	}
	else
	{
		m_currentState = &m_storedStates[animationName];
		if (m_storedStates[animationName].structs.at(0).animationResource->getFrameCount() == 1)
		{
			m_justOnePose = true;
			if (transistionTime == 0.f)
			{
				setAnimatedTransform(&(*m_storedStates[animationName].structs.at(0).animationResource->getFrames())[0]);
				applyPoseToJoints(m_rootIdx, XMMatrixIdentity());
			}
		}
		else
			m_justOnePose = false;
	}
	
	m_inBindPose = false;
}

void AnimatedMeshComponent::addSingleAnimation(std::string animationName, float transistionTime, bool playDuringStartTransistion, bool playDuringEndTransistion)
{
	animationStruct newStruct;
	newStruct.animationResource = ResourceHandler::get().loadAnimation((animationName + ".lra").c_str());
	newStruct.animationTime = 0.f;
	newStruct.animationSpeed = 1.f;
	newStruct.animationName = animationName;

	animState newState;
	newState.justOne = true;
	newState.blend = 0.f;
	newState.startTransitionDuration = transistionTime;
	newState.playDuringStartTransistion = playDuringStartTransistion;
	newState.playDuringEndTransistion = playDuringEndTransistion;
	newState.structs.push_back(newStruct);
	newState.stateName = animationName;

	m_storedStates[animationName] = newState;
}

void AnimatedMeshComponent::queueSingleAnimation(std::string animationName, float transistionTime, bool playDuringStartTransistion, bool playDuringEndTransistion)
{
	if (m_inBindPose)
	{
		playSingleAnimation(animationName, transistionTime, playDuringStartTransistion, playDuringEndTransistion);
		return;
	}

	if (m_currentState->justOne && m_animationQueue.empty() && m_currentState->structs.at(0).animationName == animationName)
		return;

	if (m_inBindPose)
	{
		ErrorLogger::get().logError("Trying to queue an animation when in bindpose, we'll just play it now. Please use the correct function.");
		playSingleAnimation(animationName, transistionTime, playDuringStartTransistion, playDuringEndTransistion);
	}

	if (m_storedStates.find(animationName) == m_storedStates.end()) // If the animation isn't in the stored states map
	{
		addSingleAnimation(animationName, transistionTime, playDuringStartTransistion, playDuringEndTransistion);
	}
	else
	{
		m_storedStates[animationName].startTransitionDuration = transistionTime; // the state is stored alrady so we'll use it and just set the transistionTime
		if (!m_storedStates[animationName].playDuringStartTransistion)
		{
			for (auto& animStruct : m_storedStates[animationName].structs)
			{
				animStruct.animationTime = 0.f;
			}
		}
	}

	m_animationQueue.push(&m_storedStates[animationName]);
}

void AnimatedMeshComponent::addBlendState(const std::initializer_list<std::pair<const std::string, float>>& animationParams, std::string stateName, bool playDuringStartTransistion, bool playDuringEndTransistion)
{
	animState newState;
	newState.justOne = false;
	newState.blend = 0.f;
	newState.playDuringStartTransistion = playDuringStartTransistion;
	newState.playDuringEndTransistion = playDuringEndTransistion;
	newState.stateName = stateName;
	
	for (auto& params : animationParams)
	{
		assert(params.first != stateName); // Assert if the state is being named the same thing as an one of it's animations. This
										   // is actually fine to do as long as that same animation doesn't exist as a single-animation-state
										   // but let's just not allow it just to save possible confusion. Please rename the state.
		animationStruct newStruct;
		newStruct.animationResource = ResourceHandler::get().loadAnimation((params.first + ".lra").c_str());
		newStruct.animationTime = 0.f;
		newStruct.animationSpeed = 1.f;
		newStruct.animationName = params.first;
		
		newState.structs.push_back(newStruct);
		newState.blendPoints.push_back(params.second);
	}

	m_storedStates[stateName] = newState;
}

void AnimatedMeshComponent::addAndPlayBlendState(const std::initializer_list<std::pair<const std::string, float>>& animationParams, std::string stateName, float transistionTime, bool playDuringStartTransistion, bool playDuringEndTransistion)
{
	addBlendState(animationParams, stateName, playDuringStartTransistion, playDuringEndTransistion);
	playBlendState(stateName, transistionTime);
}

bool AnimatedMeshComponent::playBlendState(std::string stateName, float transistionTime)
{
	// check if it is already playing
	if (!m_inBindPose)
	{
		if (m_transitionTime > 0.f || transistionTime > 0.f)
		{
			if (!m_animationQueue.empty() && m_animationQueue.front()->stateName == stateName)
				return true;
		}
		else
		{
			if (m_currentState->stateName == stateName)
				return true;
		}
	}
	
	if (m_storedStates.find(stateName) == m_storedStates.end()) // If the animation isn't in the stored states map
	{
		return false;
	}

	m_storedStates[stateName].startTransitionDuration = transistionTime;

	if (!m_storedStates[stateName].playDuringStartTransistion)
	{
		for (auto& animStruct : m_storedStates[stateName].structs)
		{
			animStruct.animationTime = 0.f;
		}
	}

	if (!m_inBindPose && transistionTime > 0.f)
	{
		m_animationQueue.push(&m_storedStates[stateName]);
		m_transitionTime = transistionTime;
	}
	else
	{
		m_currentState = &m_storedStates[stateName];
	}

	m_justOnePose = false;
	m_inBindPose = false;

	return true;
}

bool AnimatedMeshComponent::queueBlendState(std::string stateName, float transistionTime)
{
	if (m_inBindPose)
	{
		return playBlendState(stateName, transistionTime);
	}
	
	if (m_storedStates.find(stateName) == m_storedStates.end()) // If the animation isn't in the stored states map
	{
		return false;
	}

	m_storedStates[stateName].startTransitionDuration = transistionTime;

	if (!m_storedStates[stateName].playDuringStartTransistion)
	{
		for (auto& animStruct : m_storedStates[stateName].structs)
		{
			animStruct.animationTime = 0.f;
		}
	}

	m_animationQueue.push(&m_storedStates[stateName]);

	return true;
}

void AnimatedMeshComponent::setCurrentBlend(float blend)
{
	if (m_inBindPose)
		return;

	assert(!isnan(blend));

	if (m_transitionTime > 0.f)
	{
		m_animationQueue.front()->blend = blend;
	}
	else
	{
		if (m_currentState != nullptr)
		{
			m_currentState->blend = blend;
		}
	}

}

float AnimatedMeshComponent::getCurrentBlend()
{
	float returnValue = -1.0f;
	if (m_inBindPose)
	{
		return returnValue;

	}
	
	if (m_transitionTime > 0.f)
	{
		returnValue = m_animationQueue.front()->blend;
		return returnValue;
	}
	else
	{
		returnValue = m_currentState->blend;
		return returnValue;
	}


}

void AnimatedMeshComponent::applyAnimationFrame()
{
	if (m_inBindPose)
		return;

	if (
		m_animationQueue.size() > 0
		&&
		m_currentState->structs.at(0).animationTime >= m_currentState->structs.at(0).animationResource->getTimeSpan()
		&&
		m_transitionTime <= 0.0f
		)
	{
		// begin queue transition
		m_transitionTime = m_animationQueue.front()->startTransitionDuration;
		if (m_animationQueue.front()->startTransitionDuration <= 0)
			advanceQueue();
	}

	if (m_justOnePose && m_transitionTime == 0)
		return;

	for (int i = 0; i < m_currentState->structs.size(); i++)
	{
		if (m_currentState->structs.at(i).animationTime >= m_currentState->structs.at(i).animationResource->getTimeSpan())
			m_currentState->structs.at(i).animationTime -= m_currentState->structs.at(i).animationResource->getTimeSpan();
	}
	if (m_transitionTime > 0.f && m_animationQueue.front()->playDuringStartTransistion)
	{
		for (int i = 0; i < m_animationQueue.front()->structs.size(); i++)
		{
			if (m_animationQueue.front()->structs.at(i).animationTime >= m_animationQueue.front()->structs.at(i).animationResource->getTimeSpan())
				m_animationQueue.front()->structs.at(i).animationTime -= m_animationQueue.front()->structs.at(i).animationResource->getTimeSpan();
		}
	}
	
	// ---calculate current state animation frame
	ANIMATION_FRAME* finalAnimationFrame = nullptr;

	if (m_justOnePose)
		finalAnimationFrame = new ANIMATION_FRAME(*m_currentState->structs.at(0).animationResource->getFrames()[0], m_jointCount);
	else
		calculateFrameForState(m_currentState, &finalAnimationFrame);
	
	// ---transistion handling

	if (m_transitionTime > 0)
	{
		float transistionProgression = 1 - (m_transitionTime / m_animationQueue.front()->startTransitionDuration);

		ANIMATION_FRAME* inQueueFrame = nullptr;
		if (m_animationQueue.front()->playDuringStartTransistion)
		{
			calculateFrameForState(m_animationQueue.front(), &inQueueFrame);
		}
		else
		{
			ANIMATION_FRAME* allFramesOfThisAnim = *m_animationQueue.front()->structs.at(0).animationResource->getFrames();
			inQueueFrame = new ANIMATION_FRAME(allFramesOfThisAnim[0], m_jointCount);
		}

		interpolateFrame(finalAnimationFrame, inQueueFrame, transistionProgression, finalAnimationFrame);
		delete inQueueFrame;
	}

	// Apply the final frame
	setAnimatedTransform(finalAnimationFrame);
	
	delete finalAnimationFrame;

	//---apply to joints
	applyPoseToJoints(m_rootIdx, XMMatrixIdentity());
}

void AnimatedMeshComponent::calculateFrameForState(animState* state, ANIMATION_FRAME** animStateFrame)
{
	float currentBlend = 0;
	int prevBlendPoint = 0;
	
	// if state->justOne is true there is no blending necessary.

	if (!state->justOne && state->structs.size() == 2)
	{
		currentBlend = state->blend;
	}
	else if (state->structs.size() > 2)
	{
		for (int i = 0; i < state->blendPoints.size(); i++)
		{
			if (state->blend > state->blendPoints.at(i))
				break;
			prevBlendPoint = i;
		}
		float nextBlend = (prevBlendPoint + 1 == state->blendPoints.size()) ? 2.f : state->blendPoints.at(prevBlendPoint + 1);
		float deltaBlend = nextBlend - state->blendPoints.at(prevBlendPoint);
		float currentBlendBetweenPoints = state->blend - state->blendPoints.at(prevBlendPoint);
		if (deltaBlend <= 0.f)
			currentBlend = 0.0f;
		else
			currentBlend = currentBlendBetweenPoints / deltaBlend;

		assert(!isnan(currentBlend));
	}

	for (int i = prevBlendPoint; i < prevBlendPoint + 2; i++)
	{
		if (!state->justOne && i == prevBlendPoint && currentBlend == state->blendPoints.at(prevBlendPoint + 1))
		{
			continue;
		}

		AnimationResource* animResPtr = state->structs.at(i).animationResource; // just a pointer with a shorter name.
		ANIMATION_FRAME* allFramesOfThisAnim = *animResPtr->getFrames();
		ANIMATION_FRAME* thisAnimFrame = nullptr;

		int prevFrame = 0;
		int nextFrame = 0;

		for (unsigned int u = 0; u < (int)animResPtr->getFrameCount(); u++)
		{
			nextFrame = u;
			if (allFramesOfThisAnim[u].timeStamp > state->structs.at(i).animationTime)
				break;

			prevFrame = u;
		}

		float deltaTime = allFramesOfThisAnim[nextFrame].timeStamp - allFramesOfThisAnim[prevFrame].timeStamp;
		float currentTimeBetweenFrames = state->structs.at(i).animationTime - allFramesOfThisAnim[prevFrame].timeStamp;
		float progression = currentTimeBetweenFrames / deltaTime;
		if (deltaTime <= 0.f)
			progression = 0.f;

		assert(!isnan(progression));

		// now that we have the progression we can interpolate (but if the time is close enough to a timestamp (really close) we can just pick it and skip interpolation and set the current time variable)
		float allowedMargin = 0.05f;
		if (state->structs.at(i).animationSpeed != 1.f)
			allowedMargin *= pow(state->structs.at(i).animationSpeed, 0.4f);

		if (progression < 0 + allowedMargin)
			thisAnimFrame = new ANIMATION_FRAME(allFramesOfThisAnim[prevFrame], m_jointCount);
		else if (progression > 1 - allowedMargin)
		{
			thisAnimFrame = new ANIMATION_FRAME(allFramesOfThisAnim[nextFrame], m_jointCount);
			if (nextFrame != animResPtr->getFrameCount() - 1)
				state->structs.at(i).animationTime = allFramesOfThisAnim[nextFrame].timeStamp;
		}
		else
		{
			thisAnimFrame = new ANIMATION_FRAME();
			interpolateFrame(&allFramesOfThisAnim[prevFrame], &allFramesOfThisAnim[nextFrame], progression, thisAnimFrame);
		}

		if (i == prevBlendPoint)
		{
			*animStateFrame = thisAnimFrame;
			if (state->justOne || currentBlend == state->blendPoints.at(prevBlendPoint))
				break;
		}
		else
		{
			if (currentBlend == state->blendPoints.at(i))
			{
				SAFE_DELETE(*animStateFrame);
				*animStateFrame = thisAnimFrame;
			}
			else
			{
				interpolateFrame(*animStateFrame, thisAnimFrame, currentBlend, *animStateFrame);
				delete thisAnimFrame;
			}
		}
	}
}

void AnimatedMeshComponent::interpolateFrame(ANIMATION_FRAME* prevFrame, ANIMATION_FRAME* nextFrame, float progression, ANIMATION_FRAME* interpolatedFrame)
{
	//if (interpolatedFrame->jointTransforms != nullptr)
	//	delete[] interpolatedFrame->jointTransforms;
	
	if(interpolatedFrame->jointTransforms == nullptr)
		interpolatedFrame->jointTransforms = new JOINT_TRANSFORM[m_jointCount];

	for (int i = 0; i < m_jointCount; i++)
	{
		float prevTranslation[3] = { prevFrame->jointTransforms[i].translation[0], prevFrame->jointTransforms[i].translation[1], prevFrame->jointTransforms[i].translation[2] };
		float nextTranslation[3] = { nextFrame->jointTransforms[i].translation[0], nextFrame->jointTransforms[i].translation[1], nextFrame->jointTransforms[i].translation[2] };

		interpolatedFrame->jointTransforms[i].translation[0] = prevTranslation[0] + (nextTranslation[0] - prevTranslation[0]) * progression;
		interpolatedFrame->jointTransforms[i].translation[1] = prevTranslation[1] + (nextTranslation[1] - prevTranslation[1]) * progression;
		interpolatedFrame->jointTransforms[i].translation[2] = prevTranslation[2] + (nextTranslation[2] - prevTranslation[2]) * progression;

		Vector4 interpolatedQuat = XMQuaternionSlerp(Quaternion(prevFrame->jointTransforms[i].rotationQuat), Quaternion(nextFrame->jointTransforms[i].rotationQuat), progression);
		interpolatedFrame->jointTransforms[i].rotationQuat[0] = interpolatedQuat.x;
		interpolatedFrame->jointTransforms[i].rotationQuat[1] = interpolatedQuat.y;
		interpolatedFrame->jointTransforms[i].rotationQuat[2] = interpolatedQuat.z;
		interpolatedFrame->jointTransforms[i].rotationQuat[3] = interpolatedQuat.w;
	}
}

void AnimatedMeshComponent::advanceQueue()
{
	m_transitionTime = 0.f;
	m_currentState = m_animationQueue.front();
	m_animationQueue.pop();

	if (m_currentState->justOne && m_currentState->structs.at(0).animationResource->getFrameCount() == 1)
		m_justOnePose = true;
	else
		m_justOnePose = false;
}

void AnimatedMeshComponent::update(float dt)
{
	// increase animation time
	if (!m_inBindPose)
	{
		bool inTransition = (m_transitionTime > 0.f);
		
		for (int i = 0; i < m_currentState->structs.size(); i++)
		{
			bool keepPlayingCurrent = true;

			if (m_currentState->structs.at(i).animationSpeed == 0.f
				|| (inTransition && !m_currentState->playDuringEndTransistion))
				keepPlayingCurrent = false;

			if(keepPlayingCurrent)
				m_currentState->structs.at(i).animationTime += dt * m_currentState->structs.at(i).animationSpeed;
			
			if ((inTransition && !m_currentState->playDuringEndTransistion))
				m_currentState->structs.at(i).animationTime = m_currentState->structs.at(i).animationResource->getTimeSpan();
		}
		if (inTransition)
		{
			m_transitionTime -= dt;
			
			if (!m_animationQueue.empty() && m_animationQueue.front()->playDuringStartTransistion)
			{
				for (int i = 0; i < m_animationQueue.front()->structs.size(); i++)
				{
					if (m_animationQueue.front()->structs.at(i).animationSpeed > 0.f)
						m_animationQueue.front()->structs.at(i).animationTime += dt * m_animationQueue.front()->structs.at(i).animationSpeed;
				}
			}

			if (m_transitionTime <= 0)
			{
				advanceQueue();
			}
		}

		applyAnimationFrame();
	}
}

void AnimatedMeshComponent::setAnimationSpeed(const float newAnimationSpeed)
{
	if (m_inBindPose)
		return;
	
	if (m_transitionTime > 0.f) 
	{
		for (auto& animStruct : m_animationQueue.front()->structs)
		{
			animStruct.animationSpeed = newAnimationSpeed;
		}
	}
	else
	{
		for (auto& animStruct : m_currentState->structs)
		{
			animStruct.animationSpeed = newAnimationSpeed;
		}
	}
}

void AnimatedMeshComponent::setAnimationSpeed(const unsigned int structIndex, const float newAnimationSpeed)
{
	if (m_inBindPose)
		return;
	
	if (m_transitionTime > 0.f)
	{
		m_animationQueue.front()->structs.at(structIndex).animationSpeed = newAnimationSpeed;
	}
	else
	{
		m_currentState->structs.at(structIndex).animationSpeed = newAnimationSpeed;
	}
}
