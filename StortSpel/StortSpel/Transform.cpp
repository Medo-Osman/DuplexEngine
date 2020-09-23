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
	m_rotation = Vector3();
	m_scaling = Vector3();

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


void Transform::rotate(DirectX::XMFLOAT3 rotation)
{
	m_rotation = rotation;
	rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

	m_rotation += rotation;

	updated = true;
}

void Transform::translation(DirectX::XMFLOAT3 translation)
{
	m_translation = translation;
	translationMatrix = DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z);

	m_translation = translation;

	updated = true;
}

void Transform::scale(DirectX::XMFLOAT3 scaling)
{
	m_scaling = scaling;
	scalingMatrix = DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z);

	m_scaling = scaling;

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

Vector3 Transform::getRotation()
{
	return m_rotation;
}

Vector3 Transform::getTranslation()
{
	return m_translation;
}

Vector3 Transform::getScaling()
{
	return m_scaling;
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