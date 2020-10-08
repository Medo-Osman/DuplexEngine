#include "3DPCH.h"
#include "Transform.h"


Transform::Transform()
{
	setTransformZero();
	translationLock = false;
	rotationLock = false;
	scaleLock = false;
}

Transform::~Transform()
{}

void Transform::setTransformZero()
{
	translationMatrix = DirectX::XMMatrixTranslation(0, 0, 0);
	rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(0, 0, 0);
	scalingMatrix = DirectX::XMMatrixScaling(1, 1, 1);

	m_translation = Vector3();
	m_rotationQuat = Quaternion();
	m_scaling = { 1, 1, 1 };
	
	updated = true;
}

void Transform::move(DirectX::XMVECTOR moveVector)
{
	if (translationLock)
		return;

	m_translation += moveVector;
	translationMatrix = DirectX::XMMatrixMultiply(translationMatrix, DirectX::XMMatrixTranslationFromVector(moveVector));

	updated = true;
}


void Transform::rotate(Vector3 rotation)
{
	m_rotationQuat = Quaternion::CreateFromYawPitchRoll(rotation.y, rotation.x, rotation.z);
	m_rotationQuat.Normalize();

	rotationMatrix = DirectX::XMMatrixRotationQuaternion(m_rotationQuat); //DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

	updated = true;
}

void Transform::translation(DirectX::XMFLOAT3 translation)
{
	m_translation = translation;
	translationMatrix = DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z);

	updated = true;
}

void Transform::scale(DirectX::XMFLOAT3 scaling)
{
	m_scaling = scaling;
	scalingMatrix = DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z);


	updated = true;
}



void Transform::scaleUniform(float amount)
{
	if (scaleLock)
		return;

	scalingMatrix = DirectX::XMMatrixMultiply(scalingMatrix, DirectX::XMMatrixScaling(amount, amount, amount));
	m_scaling = Vector3(amount, amount, amount);
	
	updated = true;
}

void Transform::setTranslationLock(bool state)
{
	translationLock = state;
}

void Transform::setRotationLock(bool state)
{
	rotationLock = state;
}

void Transform::setScaleLock(bool state)
{
	scaleLock = state;
}

Transform* Transform::getTransform()
{
	return this;
}

Quaternion Transform::getRotation()
{
	return m_rotationQuat;
}

Vector3 Transform::getTranslation()
{
	return m_translation;
}

Vector3 Transform::getScaling()
{
	return m_scaling;
}

void Transform::setQuaternion(Quaternion quat)
{
	m_rotationQuat.Normalize();
	m_rotationQuat = quat;

	Matrix quatRotMatrix = XMMatrixRotationQuaternion(m_rotationQuat);
	rotationMatrix = quatRotMatrix;

	updated = true;
}

Quaternion Transform::getQuaternion()
{
	return m_rotationQuat;
}

Vector4 Transform::getVec4Quaternion()
{
	Quaternion tempQuat = m_rotationQuat;//Quaternion(0, m_rotationQuat.y, 0, 0);
	return tempQuat;
}

Vector3 Transform::getForwardVector()
{
	return XMVector3Rotate(Vector3(0, 0, 1), getVec4Quaternion());
}

Vector3 Transform::getUpVector()
{
	return XMVector3Rotate(Vector3(0, 1, 0), getVec4Quaternion());
}

Vector3 Transform::getRightVector()
{
	return XMVector3Rotate(Vector3(1, 0, 0), getVec4Quaternion());
}

Vector3 Transform::getBackwardVector()
{
	return XMVector3Rotate(Vector3(0, 0, -1), getVec4Quaternion());
}

Matrix Transform::getRotationMatrix()
{
	return rotationMatrix;
}

DirectX::XMMATRIX Transform::calculateWorldMatrix()
{
	DirectX::XMMATRIX worldMtx = scalingMatrix * rotationMatrix * translationMatrix;
	return worldMtx;
}

bool Transform::getUpdated() const
{
	return updated;
}

void Transform::setNotUpdated()
{
	updated = false;
}