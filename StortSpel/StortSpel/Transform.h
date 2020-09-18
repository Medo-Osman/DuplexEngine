#pragma once
#include <DirectXColors.h>
using namespace DirectX;

class Transform
{
private:

	XMMATRIX translation;
	XMMATRIX rotation;
	XMMATRIX scaling;

	bool updated;

	bool translationLock;
	bool rotationLock;
	bool scaleLock;

public:
	Transform();
	~Transform();

	void setTransformZero();

	void move(DirectX::XMVECTOR moveVector);
	void rotate(DirectX::XMMATRIX rotationMatrix);
	void rotate(float x, float y, float z);
	void scale(float x, float y, float z);
	void scaleUniform(float amount);

	void setTranslationLock(bool state);
	void setRotationLock(bool state);
	void setScaleLock(bool state);

	DirectX::XMMATRIX calculateWorldMatrix();

	bool getUpdated() const;
	void setNotUpdated();
};