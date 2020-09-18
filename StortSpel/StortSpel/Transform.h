#pragma once
#include <DirectXColors.h>


class Transform
{
private:

	DirectX::XMMATRIX translation;
	DirectX::XMMATRIX rotation;
	DirectX::XMMATRIX scaling;

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