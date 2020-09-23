#pragma once
#include <DirectXColors.h>
#include "SimpleMath.h"




using namespace DirectX;
using namespace SimpleMath;


class Transform
{
private:

	XMMATRIX translationMatrix;
	XMMATRIX rotationMatrix;
	XMMATRIX scalingMatrix;

	Vector3 m_scaling;
	Vector3 m_rotation;
	Vector3 m_translation;

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

	//sets rotation/translation/scale
	void rotate(DirectX::XMFLOAT3 rotation);
	void translation(DirectX::XMFLOAT3 translation);
	void scale(DirectX::XMFLOAT3 scaling);


	void scaleUniform(float amount);

	void setTranslationLock(bool state);
	void setRotationLock(bool state);
	void setScaleLock(bool state);

	Vector3 getRotation();
	Vector3 getTranslation();
	Vector3 getScaling();

	DirectX::XMMATRIX calculateWorldMatrix();

	bool getUpdated() const;
	void setNotUpdated();
};