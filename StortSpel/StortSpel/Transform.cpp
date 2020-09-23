#include "3DPCH.h"
#include "Transform.h"



Transform::Transform()
{
	this->setTransformZero();
	this->translationLock = false;
	this->rotationLock = false;
	this->scaleLock = false;
}

Transform::~Transform()
{}

void Transform::setTransformZero()
{
	translation = DirectX::XMMatrixTranslation(0, 0, 0);
	rotation = DirectX::XMMatrixRotationRollPitchYaw(0, 0, 0);
	scaling = DirectX::XMMatrixScaling(1, 1, 1);

	this->updated = true;
}

void Transform::move(DirectX::XMVECTOR moveVector)
{
	if (this->translationLock)
		return;

	this->translation = DirectX::XMMatrixMultiply(translation, DirectX::XMMatrixTranslationFromVector(moveVector));

	this->updated = true;
}

void Transform::rotate(DirectX::XMMATRIX rotationMatrix)
{
	if (this->rotationLock)
		return;

	this->rotation = DirectX::XMMatrixMultiply(rotation, rotationMatrix);

	this->updated = true;
}

void Transform::rotate(float x, float y, float z)
{
	if (this->rotationLock)
		return;

	this->rotation = DirectX::XMMatrixMultiply(rotation, DirectX::XMMatrixRotationRollPitchYaw(x, y, z));

	this->updated = true;
}

void Transform::scale(float x, float y, float z)
{
	if (this->scaleLock)
		return;

	this->scaling = DirectX::XMMatrixMultiply(scaling, DirectX::XMMatrixScaling(x, y, z));

	this->updated = true;
}

void Transform::scaleUniform(float amount)
{
	if (this->scaleLock)
		return;

	this->scaling = DirectX::XMMatrixMultiply(scaling, DirectX::XMMatrixScaling(amount, amount, amount));

	this->updated = true;
}

void Transform::setTranslationLock(bool state)
{
	this->translationLock = state;
}

void Transform::setRotationLock(bool state)
{
	this->rotationLock = state;
}

void Transform::setScaleLock(bool state)
{
	this->scaleLock = state;
}

DirectX::XMMATRIX Transform::calculateWorldMatrix()
{
	DirectX::XMMATRIX worldMtx = scaling * rotation * translation;
	//worldMtx = DirectX::XMMatrixTranspose(worldMtx);
	return worldMtx;
}

bool Transform::getUpdated() const
{
	return this->updated;
}

void Transform::setNotUpdated()
{
	this->updated = false;
}