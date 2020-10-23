#include "3DPCH.h"
#include "AnimatedMeshComponent.h"

AnimatedMeshComponent::AnimatedMeshComponent(const char* filepath, ShaderProgramsEnum shaderEnum, Material material)
	:MeshComponent(shaderEnum, material)
{
	SkeletalMeshResource* resPtr = (SkeletalMeshResource*)ResourceHandler::get().loadLRSMMesh(filepath);
	
	setMeshResourcePtr(resPtr);

	//take the joints from the meshresource and build the joints
	m_jointCount = resPtr->getJointCount();

	//std::vector<joint> joints;

	LRSM_JOINT* LRSMJoints = resPtr->getJoints();

	m_rootJoint = createJointAndChildren(0, -1, resPtr->getJoints());

	calcInverseBindTransform(&m_rootJoint, XMMatrixIdentity());

	// when the temp rotation values go this can't be here but the cuffer stuff will prob need sometinh else
	applyPoseToJoints(&m_rootJoint, XMMatrixIdentity());

	m_currentAnimationResource = nullptr;

	//calcInverseBindTransform(&m_rootJoint, m_rootJoint.localBindTransform);
}

AnimatedMeshComponent::AnimatedMeshComponent(const char* filepath, Material material)
	: AnimatedMeshComponent(filepath, ShaderProgramsEnum::DEFAULT, material)
{}

joint AnimatedMeshComponent::createJointAndChildren(int currentIndex, int parentIndex, LRSM_JOINT* LRSMJoint)
{
	joint thisJoint;

	thisJoint.index = currentIndex;
	
	//int f[3] = { 0, 1, 2 };
	int f[3] = { 1, 0, 2 };

	Quaternion rotationQuat = XMQuaternionRotationRollPitchYaw(
		XMConvertToRadians(LRSMJoint[currentIndex].rotation[f[0]]),
		XMConvertToRadians(LRSMJoint[currentIndex].rotation[f[1]]),
		XMConvertToRadians(LRSMJoint[currentIndex].rotation[f[2]])
	);
	

	Quaternion rotationQuat1 = XMQuaternionRotationRollPitchYaw(
		XMConvertToRadians(LRSMJoint[currentIndex].tempPoseRotation[f[0]]),
		XMConvertToRadians(LRSMJoint[currentIndex].tempPoseRotation[f[1]]),
		XMConvertToRadians(LRSMJoint[currentIndex].tempPoseRotation[f[2]])
	);
	
	//if(currentIndex == 0)
	//	rotationQuat = Quaternion::CreateFromYawPitchRoll(0, 0, 0);
	
	//rotationQuat.Normalize();
	//rotationQuat1.Normalize();

	XMMATRIX rotationMatrix =  DirectX::XMMatrixRotationQuaternion(rotationQuat);
	XMMATRIX rotationMatrix1 = XMMatrixMultiply( DirectX::XMMatrixRotationQuaternion(rotationQuat), DirectX::XMMatrixRotationQuaternion(rotationQuat1) );
	//XMMATRIX rotationMatrix1 = XMMatrixMultiply(DirectX::XMMatrixRotationQuaternion(rotationQuat1), DirectX::XMMatrixRotationQuaternion(rotationQuat) );

	XMVECTOR v = XMVectorSet(LRSMJoint[currentIndex].translation[0], LRSMJoint[currentIndex].translation[1], LRSMJoint[currentIndex].translation[2], 1);
	
	Vector3 v1 = XMVector4Transform(v, rotationMatrix);
	XMVECTOR v2 = XMVector4Transform(v, rotationMatrix1);

	XMMATRIX ro;

	ro = 
		XMMatrixRotationX(XMConvertToRadians(LRSMJoint[currentIndex].rotation[0])) 
		*
		XMMatrixRotationY(XMConvertToRadians(LRSMJoint[currentIndex].rotation[1])) 
		*
		XMMatrixRotationZ(XMConvertToRadians(LRSMJoint[currentIndex].rotation[2]))
		;

	XMMATRIX ro1;

	/*ro1 =
		XMMatrixRotationX(XMConvertToRadians(LRSMJoint[currentIndex].tempPoseRotation[0]))
		*
		XMMatrixRotationY(XMConvertToRadians(LRSMJoint[currentIndex].tempPoseRotation[1]))
		*
		XMMatrixRotationZ(XMConvertToRadians(LRSMJoint[currentIndex].tempPoseRotation[2]))
		;*/
	
	//Quaternion tempPoseRotationQuat = XMQuaternionRotationMatrix(ro1);
	Quaternion tempPoseRotationQuat = Quaternion(LRSMJoint[currentIndex].tempPoseRotation[0], LRSMJoint[currentIndex].tempPoseRotation[1], LRSMJoint[currentIndex].tempPoseRotation[2], LRSMJoint[currentIndex].tempPoseRotation[3]);

	ro1 = DirectX::XMMatrixRotationQuaternion(tempPoseRotationQuat);
	//ro1 = ro1 * ro;

	thisJoint.localBindTransform = XMMatrixMultiply(
		//XMMatrixRotationRollPitchYaw(LRSMJoint[currentIndex].rotation[0], LRSMJoint[currentIndex].rotation[1], LRSMJoint[currentIndex].rotation[2])
		//XMMatrixRotationRollPitchYaw(LRSMJoint[currentIndex].rotation[0], LRSMJoint[currentIndex].rotation[2], LRSMJoint[currentIndex].rotation[1])
		//XMMatrixRotationRollPitchYaw(LRSMJoint[currentIndex].rotation[1], LRSMJoint[currentIndex].rotation[0], LRSMJoint[currentIndex].rotation[2])
		//XMMatrixRotationRollPitchYaw(LRSMJoint[currentIndex].rotation[1], LRSMJoint[currentIndex].rotation[2], LRSMJoint[currentIndex].rotation[0])
		//XMMatrixRotationRollPitchYaw(LRSMJoint[currentIndex].rotation[2], LRSMJoint[currentIndex].rotation[0], LRSMJoint[currentIndex].rotation[1])
		//XMMatrixRotationRollPitchYaw(LRSMJoint[currentIndex].rotation[2], LRSMJoint[currentIndex].rotation[1], LRSMJoint[currentIndex].rotation[0])
		//rotationMatrix
		//XMMatrixRotationRollPitchYaw(0, 0, XMConvertToRadians(LRSMJoint[currentIndex].rotation[2]))
		/*XMMatrixRotationRollPitchYaw(
			XMConvertToRadians(LRSMJoint[currentIndex].rotation[f[0]]),
			XMConvertToRadians(LRSMJoint[currentIndex].rotation[f[1]]),
			XMConvertToRadians(LRSMJoint[currentIndex].rotation[f[2]])
		)*/
		ro
		, 
		XMMatrixTranslation(LRSMJoint[currentIndex].translation[0], LRSMJoint[currentIndex].translation[1], LRSMJoint[currentIndex].translation[2]) 
	);

	//thisJoint.localBindTransform = XMMatrixTranslationFromVector(v1);
	
	//thisJoint.localBindTransform = XMMatrixTranspose(thisJoint.localBindTransform);
	XMMATRIX tempPoseRotationMat = XMMatrixIdentity();
	
	/*if (parentIndex > -1)
	{
		XMMATRIX p = XMMatrixRotationRollPitchYaw(
			XMConvertToRadians(LRSMJoint[parentIndex].rotation[f[0]]),
			XMConvertToRadians(LRSMJoint[parentIndex].rotation[f[1]]),
			XMConvertToRadians(LRSMJoint[parentIndex].rotation[f[2]])
		);
		XMMATRIX p1 = XMMatrixRotationRollPitchYaw(
			XMConvertToRadians(-LRSMJoint[parentIndex].tempPoseRotation[f[0]]),
			XMConvertToRadians(-LRSMJoint[parentIndex].tempPoseRotation[f[1]]),
			XMConvertToRadians(-LRSMJoint[parentIndex].tempPoseRotation[f[2]])
		);
		
		tempPoseRotationMat = XMMatrixMultiply(
			p,
			p1
		);
	}*/

	

	/*tempPoseRotationMat =
		XMMatrixRotationRollPitchYaw(
			XMConvertToRadians(LRSMJoint[currentIndex].tempPoseRotation[f[0]]),
			XMConvertToRadians(LRSMJoint[currentIndex].tempPoseRotation[f[1]]),
			XMConvertToRadians(LRSMJoint[currentIndex].tempPoseRotation[f[2]])
		);*/
		

	XMMATRIX rot = XMMatrixMultiply(
		
		XMMatrixRotationRollPitchYaw(
			XMConvertToRadians(LRSMJoint[currentIndex].tempPoseRotation[f[0]]),
			XMConvertToRadians(LRSMJoint[currentIndex].tempPoseRotation[f[1]]),
			XMConvertToRadians(LRSMJoint[currentIndex].tempPoseRotation[f[2]])
		)
		,
		XMMatrixRotationRollPitchYaw(
			XMConvertToRadians(LRSMJoint[currentIndex].rotation[f[0]]),
			XMConvertToRadians(LRSMJoint[currentIndex].rotation[f[1]]),
			XMConvertToRadians(LRSMJoint[currentIndex].rotation[f[2]])
		)
		
	);

	rot = XMMatrixRotationQuaternion(rotationQuat1 * rotationQuat);
	rot = rotationMatrix1;

	// use animatedtransform to store stuff, consider how the transforms need to work in reletion to the vertices
	thisJoint.animatedTransform = XMMatrixMultiply(
		ro1
		,
		XMMatrixTranslation(LRSMJoint[currentIndex].translation[0], LRSMJoint[currentIndex].translation[1], LRSMJoint[currentIndex].translation[2]) 
	);

	//thisJoint.animatedTransform = XMMatrixTranslationFromVector(v2);

	//thisJoint.animatedTransform = XMMatrixTranspose(thisJoint.animatedTransform);
	//temp:

	//m_cBufferStruct.boneMatrixPallet[thisJoint.index] = thisJoint.localBindTransform;
	
	/*float x = XMVectorGetX(LRSMJoint[currentIndex].translation);
	float y = XMVectorGetY(LRSMJoint[currentIndex].translation);
	float z = XMVectorGetZ(LRSMJoint[currentIndex].translation);
	float w = XMVectorGetW(LRSMJoint[currentIndex].translation);*/

	for (int i = 0; i < LRSMJoint[currentIndex].nrOfChildren; i++)
	{
		thisJoint.children.push_back(createJointAndChildren(LRSMJoint[currentIndex].children[i], currentIndex, LRSMJoint));
	}

	return thisJoint;
}

void AnimatedMeshComponent::calcInverseBindTransform(joint* thisJoint, Matrix parentBindTransform)
{
	Matrix bindTransform = XMMatrixMultiply(thisJoint->localBindTransform, parentBindTransform );
	
	//Vector4 testVec = XMVector3Transform(Vector3(0, 0, 0), bindTransform);
	//Vector4 testVec1 = XMVector3Transform(Vector3(0, 0, 0), thisJoint->localBindTransform);
	
	thisJoint->inverseBindTransform = bindTransform.Invert();
	
	// temp:
	//thisJoint->animatedTransform = bindTransform;
	//m_cBufferStruct.boneMatrixPallet[thisJoint->index] = bindTransform;
	//m_cBufferStruct.boneMatrixPallet[thisJoint->index] = thisJoint->localBindTransform;
	
	for (int i = 0; i < thisJoint->children.size(); i++)
	{
		calcInverseBindTransform(&thisJoint->children.at(i), bindTransform);
	}
	
	//Temp: (for the time being thisJoint->animatedTransform = currentlocaltransform)
	//Matrix currentTransform =  thisJoint->animatedTransform

	//m_cBufferStruct.boneMatrixPallet[thisJoint->index] = thisJoint->animatedTransform * thisJoint->inverseBindTransform;

	// Basically i think the rotations coming in are not the bind pose, it's the bindpose plus the pose the model had.
	// I want to test this by doing a quick calculation of the matrices that will be sent to the shader and trying the shader.
	// If i can get her to do the pose then my sus will be correct and I will know that everything in this program works as intended
	// look at specifically https://youtu.be/cieheqt7eqc?t=464 to get started on a temp setting of thisJoint->animatedTransform;
	// Or maybe try just setting the "(bind)Transform" in this function as thisJoint->animatedTransform and try passing in the values.

	// I fiddled around a bunch and found several ways that seem right but don't work, one more thing to try is to look at the link above again and se if the inverse bindtransform step is needed to test this

	//thisJoint->inverseBindTransform = XMMatrixInverse(bindTransform);
		//TODO: copy from https://www.youtube.com/watch?v=F-kcaonjHf8 
		   // but remember that we aren't in the joint like the video so we need to send in a joint.
		   // also remember that openGL uses diffrent martices, maybe.
}

void AnimatedMeshComponent::setAnimatedTransform(joint* thisJoint, ANIMATION_FRAME* animationFrame)
{
	// THIS whole thing IS TEMP, THERE IS NO INTERPOLATION
	
	//JOINT_TRANSFORM jt = animationFrame->jointTransforms[thisJoint->index];
	
	//DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::SimpleMath::Quaternion(jt.rotationQuat[0], jt.rotationQuat[1], jt.rotationQuat[2], jt.rotationQuat[4]));
	//thisJoint->animatedTransform = XMMatrixMultiply(rotationMatrix, XMMatrixTranslation(jt.translation[0], jt.translation[1], jt.translation[2]));
	thisJoint->animatedTransform = animationFrame->jointTransforms[thisJoint->index].asMatrix();
	
	for (int i = 0; i < thisJoint->children.size(); i++)
	{
		setAnimatedTransform(&thisJoint->children.at(i), animationFrame);
	}
}

void AnimatedMeshComponent::applyPoseToJoints(joint* thisJoint, Matrix parentTransform)
{
	//Temp: (for the time being thisJoint->animatedTransform = currentlocaltransform)
	//Matrix currentTransform = parentTransform * thisJoint->animatedTransform;
	Matrix currentTransform = XMMatrixMultiply(thisJoint->animatedTransform, parentTransform );

	//Vector4 testVec = XMVector3Transform(Vector3(0, 0, 0), currentTransform);

	for (int i = 0; i < thisJoint->children.size(); i++)
	{
		applyPoseToJoints(&thisJoint->children.at(i), currentTransform);
	}
	//m_cBufferStruct.boneMatrixPallet[thisJoint->index] = currentTransform * thisJoint->inverseBindTransform;
	m_cBufferStruct.boneMatrixPallet[thisJoint->index] = XMMatrixTranspose( XMMatrixMultiply(thisJoint->inverseBindTransform, currentTransform ) );
	//m_cBufferStruct.boneMatrixPallet[thisJoint->index] = thisJoint->animatedTransform * thisJoint->inverseBindTransform;
}
// for when i need it: https://youtu.be/HeIkk6Yo0s8?t=701

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
	m_currentFrame = 0;

	// call a apply animationFrame function on the first frame (i guess, i don't suppose we'll add blending of some sort later)
	applyAnimationFrame();
}

void AnimatedMeshComponent::applyAnimationFrame()
{
	if (m_currentAnimationResource == nullptr || m_isDone)
		return;
	
	// check if the animation has ended, then either do some looping or set it to be the last frame (maybe there can be an animation playlist/sequence feature or something)
	if (m_animationTime > m_currentAnimationResource->getTimeSpan())
	{
		if (m_shouldLoop)
			m_animationTime = m_animationTime - m_currentAnimationResource->getTimeSpan();
		else
		{
			m_animationTime = m_currentAnimationResource->getTimeSpan();
			m_isDone = true;
			//m_currentFrame = m_currentAnimationResource->getFrameCount();
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

	setAnimatedTransform(&m_rootJoint, &(allFrames[prevFrame]));

	
	//if (m_currentFrame == m_currentAnimationResource->getFrameCount())
	//{
	//	// no progression, just set to last frame
	//}
	//else if (m_animationTime > allFrames[m_currentFrame + 1].timeStamp)
	//{
	//	for (int i = m_currentFrame + 1; i < m_currentAnimationResource->getFrameCount(); i++)
	//	{

	//	}
	//}
	//else
	//{
	//	prevFrame = m_currentFrame;
	//	nextFrame = m_currentFrame + 1;
	//}
	// if the current time is more than the next frames time, then search after the correct previous and next frames

	// now that we have the progression we can interpolate (but if the time is close enough to a timestamp (really close) we can just pick it and skip interpolation (will need testing) and set the current time variable)

	// then when we have the correct translation (don't forget to consider removing the translations or making them optional) and rotation we can caluclate the matriacies

	

	//---apply to joints
	applyPoseToJoints(&m_rootJoint, XMMatrixIdentity());
	// recursivly set all the animatedTransform for all the joints
	// or set them in the cbuffer thing with their index.
}

void AnimatedMeshComponent::update(float dt)
{
	// increase animation time
	m_animationTime += dt;
	
	applyAnimationFrame();
}
