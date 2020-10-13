#pragma once
#include <DirectXColors.h>

using namespace DirectX;
using namespace SimpleMath;


class Transform
{
private:

	XMMATRIX translationMatrix;
	XMMATRIX rotationMatrix;
	XMMATRIX scalingMatrix;

	Vector3 m_scaling;
	//Vector3 m_rotation;
	Vector3 m_translation;
	Quaternion m_rotationQuat;


	Vector3 m_forwardVector;
	Vector3 m_upVector;
	Vector3 m_rightVector;
	Vector3 m_backVector;

	bool updated;

	bool translationLock;
	bool rotationLock;
	bool scaleLock;

public:
	Transform();
	~Transform();

	void setTransformZero();

	void move(DirectX::XMVECTOR moveVector);

	//sets rotation/translation/scale
	void rotate(Vector3 rotation);
	void rotateQuat(Quaternion rotation);
	void translation(DirectX::XMFLOAT3 translation);
	void scale(DirectX::XMFLOAT3 scaling);


	void scaleUniform(float amount);

	void setTranslationLock(bool state);
	void setRotationLock(bool state);
	void setScaleLock(bool state);


	Transform* getTransform();
	Quaternion getRotation();

	Vector3 getTranslation();
	Vector3 getScaling();

	void setQuaternion(Quaternion quat);
	Quaternion getQuaternion();

	Vector4 getVec4Quaternion();


	Vector3 getForwardVector();
	Vector3 getUpVector();
	Vector3 getRightVector();
	Vector3 getBackwardVector();

	Matrix getRotationMatrix();


	DirectX::XMMATRIX calculateWorldMatrix();

	bool getUpdated() const;
	void setNotUpdated();
};