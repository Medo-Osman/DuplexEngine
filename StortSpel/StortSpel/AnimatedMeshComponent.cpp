#include "3DPCH.h"
#include "AnimatedMeshComponent.h"

AnimatedMeshComponent::AnimatedMeshComponent(const char* filepath, ShaderProgramsEnum shaderEnum, Material material)
	:MeshComponent(shaderEnum, material), m_animationSpeed(1.0f), m_currentAnimationResource(nullptr)
{
	SkeletalMeshResource* resPtr = (SkeletalMeshResource*)ResourceHandler::get().loadLRSMMesh(filepath);
	
	setMeshResourcePtr(resPtr);

	//take the joints from the meshresource and build the joints
	m_jointCount = resPtr->getJointCount();

	m_joints.reserve(m_jointCount);
	for (int i = 0; i < m_jointCount; i++)
	{
		m_joints.push_back(joint());
	}

	m_rootIdx = resPtr->getRootIndex();
	m_joints.at(m_rootIdx) = createJointAndChildren(m_rootIdx, resPtr->getJoints());

	calcInverseBindTransform(m_rootIdx, XMMatrixIdentity());
	
	for (int i = 0; i < m_jointCount; i++)
	{
		m_cBufferStruct.boneMatrixPallet[i] = XMMatrixIdentity();
	}

	// when the temp rotation values go this might not need to be here
	//applyPoseToJoints(m_rootIdx, XMMatrixIdentity());
}

AnimatedMeshComponent::AnimatedMeshComponent(const char* filepath, Material material)
	: AnimatedMeshComponent(filepath, ShaderProgramsEnum::DEFAULT, material)
{}

joint AnimatedMeshComponent::createJointAndChildren(int currentIndex, LRSM_JOINT* LRSMJoint)
{
	joint thisJoint;

	// Set the joint's index
	thisJoint.index = currentIndex;
	
	// Calculate the localBindTransform using
	XMMATRIX localBindRotationMatrix =
		XMMatrixRotationX(XMConvertToRadians(LRSMJoint[currentIndex].rotation[0])) 
		*
		XMMatrixRotationY(XMConvertToRadians(LRSMJoint[currentIndex].rotation[1])) 
		*
		XMMatrixRotationZ(XMConvertToRadians(LRSMJoint[currentIndex].rotation[2]))
		;

	thisJoint.localBindTransform = XMMatrixMultiply(
		localBindRotationMatrix, 
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
	
	//thisJoint.animatedTransform = thisJoint.localBindTransform;

	for (int i = 0; i < LRSMJoint[currentIndex].nrOfChildren; i++)
	{
		thisJoint.children.push_back(LRSMJoint[currentIndex].children[i]);
		m_joints.at(LRSMJoint[currentIndex].children[i]) = createJointAndChildren(LRSMJoint[currentIndex].children[i], LRSMJoint);
	}

	return thisJoint;
}

void AnimatedMeshComponent::calcInverseBindTransform(int thisJointIdx, Matrix parentBindTransform)
{
	Matrix bindTransform = XMMatrixMultiply(m_joints[thisJointIdx].localBindTransform, parentBindTransform );
	
	Vector4 testVec = XMVector3Transform(Vector3(0, 0, 0), bindTransform);
	Vector4 testVec1 = XMVector3Transform(Vector3(0, 0, 0), m_joints[thisJointIdx].localBindTransform);
	
	m_joints[thisJointIdx].inverseBindTransform = bindTransform.Invert();
	
	for (int i = 0; i < m_joints[thisJointIdx].children.size(); i++)
	{
		calcInverseBindTransform(m_joints[thisJointIdx].children.at(i), bindTransform);
	}
}

void AnimatedMeshComponent::setAnimatedTransform(int thisJointIdx, ANIMATION_FRAME* animationFrame)
{
	m_joints[thisJointIdx].animatedTransform = animationFrame->jointTransforms[thisJointIdx].asMatrix();
	
	for (int i = 0; i < m_joints[thisJointIdx].children.size(); i++)
	{
		setAnimatedTransform(m_joints[thisJointIdx].children.at(i), animationFrame);
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

void AnimatedMeshComponent::playAnimation(std::string animationName, bool looping)
{
	// get/load the animation from the resource handler (might want to have a seperate preload function as well)
	m_currentAnimationResource = ResourceHandler::get().loadAnimation( (animationName + ".lra").c_str() );

	// set some variables that will allow the animation to play on it's own in an update function.
	m_animationTime = 0;
	m_shouldLoop = looping;
	m_isDone = false;

	// call a apply animationFrame function on the first frame (i guess, i don't suppose we'll add blending of some sort later)
	applyAnimationFrame();
}

void AnimatedMeshComponent::applyAnimationFrame()
{
	if (m_currentAnimationResource == nullptr || m_isDone)
		return;
	
	if (m_currentAnimationResource->getFrameCount() == 1) // only one frame means it's a pose
	{
		setAnimatedTransform(m_rootIdx, &(*m_currentAnimationResource->getFrames())[0] );
		applyPoseToJoints(m_rootIdx, XMMatrixIdentity());
	}

	// check if the animation has ended, then either do some looping or set it to be the last frame (maybe there can be an animation playlist/sequence feature or something)
	if (m_animationTime > m_currentAnimationResource->getTimeSpan())
	{
		if (m_shouldLoop)
			m_animationTime = m_animationTime - m_currentAnimationResource->getTimeSpan();
		else
		{
			m_animationTime = m_currentAnimationResource->getTimeSpan();
			m_isDone = true;
		}	
	}

	// ---calculate current pose

	ANIMATION_FRAME* allFrames = *m_currentAnimationResource->getFrames();

	// get distance (progression) from current frame to the next frame

	int prevFrame = 0;
	int nextFrame = 0;

	for (int i = 0; i < m_currentAnimationResource->getFrameCount(); i++)
	{
		nextFrame = i;
		if (allFrames[i].timeStamp > m_animationTime)
			break;
		prevFrame = i;
	}

	float deltaTime = allFrames[nextFrame].timeStamp - allFrames[prevFrame].timeStamp;
	float currentTimeBetweenFrames = m_animationTime - allFrames[prevFrame].timeStamp;
	float progression = currentTimeBetweenFrames / deltaTime;

	// now that we have the progression we can interpolate (but if the time is close enough to a timestamp (really close) we can just pick it and skip interpolation and set the current time variable)
	float allowedMargin = 0.05f;
	allowedMargin * pow(m_animationSpeed, 0.4);

	if (progression < 0 + allowedMargin)
		setAnimatedTransform(m_rootIdx, &(allFrames[prevFrame]));
	else if (progression > 1 - allowedMargin)
	{
		setAnimatedTransform(m_rootIdx, &(allFrames[nextFrame]));
		m_animationTime = allFrames[nextFrame].timeStamp;
	}
	else
	{
		ANIMATION_FRAME interpolatedFrame;

		interpolatedFrame.jointTransforms = new JOINT_TRANSFORM[m_jointCount];

		for (int i = 0; i < m_jointCount; i++)
		{
			float prevTranslation[3] = { allFrames[prevFrame].jointTransforms[i].translation[0], allFrames[prevFrame].jointTransforms[i].translation[1], allFrames[prevFrame].jointTransforms[i].translation[2] };
			float nextTranslation[3] = { allFrames[nextFrame].jointTransforms[i].translation[0], allFrames[nextFrame].jointTransforms[i].translation[1], allFrames[nextFrame].jointTransforms[i].translation[2] };
			
			interpolatedFrame.jointTransforms[i].translation[0] = prevTranslation[0] + (nextTranslation[0] - prevTranslation[0]) * progression;
			interpolatedFrame.jointTransforms[i].translation[1] = prevTranslation[1] + (nextTranslation[1] - prevTranslation[1]) * progression;
			interpolatedFrame.jointTransforms[i].translation[2] = prevTranslation[2] + (nextTranslation[2] - prevTranslation[2]) * progression;
			
			Vector4 interpolatedQuat = XMQuaternionSlerp(Quaternion(allFrames[prevFrame].jointTransforms[i].rotationQuat), Quaternion(allFrames[nextFrame].jointTransforms[i].rotationQuat), progression);
			interpolatedFrame.jointTransforms[i].rotationQuat[0] = interpolatedQuat.x;
			interpolatedFrame.jointTransforms[i].rotationQuat[1] = interpolatedQuat.y;
			interpolatedFrame.jointTransforms[i].rotationQuat[2] = interpolatedQuat.z;
			interpolatedFrame.jointTransforms[i].rotationQuat[3] = interpolatedQuat.w;
		}

		setAnimatedTransform(m_rootIdx, &interpolatedFrame);

		delete[] interpolatedFrame.jointTransforms;
	}

	//---apply to joints
	applyPoseToJoints(m_rootIdx, XMMatrixIdentity());
}

void AnimatedMeshComponent::update(float dt)
{
	// increase animation time
	m_animationTime += dt * m_animationSpeed;
	
	applyAnimationFrame();
}
