#pragma once
#include <DirectXColors.h>
#include "SimpleMath.h"

using namespace DirectX;
using namespace SimpleMath;


class Transform
{
private: // Transformation variables

	/* Position vector. */
	Vector3 m_translation;

	/* Rotation quaternion. */
	Quaternion m_rotation;

	/* Scale vector. */
	Vector3 m_scale;

private: // Cache
	/* Cached transformation matrix aka. the world matrix or the final matrix. */
	XMMATRIX m_transform;

	/* Cached translation matrix. Cache state is indicated by "m_translateDirty" */
	XMMATRIX m_translationMat;

	/* Cache state for `m_TranslationMat`. */
	bool m_translationDirty;

	/* Cached rotation matrix. Cache state is indicated by "m_rotationDirty" */
	XMMATRIX m_rotationMat;

	/* Cache state for `m_RotationMat` */
	bool m_rotationDirty;

	/* Cached scaling matrix. Cache state is indicated by "m_scaleDirty". */
	XMMATRIX m_scaleMat;

	/* Cache state for "m_ScaleMat". */
	bool m_scaleDirty;

public:
	Transform();
	virtual ~Transform();

public:
	/* Generate object transformation matrix. */
	virtual XMMATRIX calculateWorldMatrix();

	/* Generate object transformation matrix multiplied with supplied matrix. */
	virtual XMMATRIX calculateWorldMatrix(XMMATRIX mat);

	/* Generate object translation matrix. */
	virtual XMMATRIX getTranslationMatrix();
	
	/* Generate object rotation matrix. */
	virtual XMMATRIX getRotationMatrix();
	
	/* Generate object scaling matrix. */
	virtual XMMATRIX getScaleMatrix();

public:
	/* Set position to (0,0,0). */
	void setPositionZero();

	/* Set position by vector. */
	void setPosition(const Vector3& position);

	/* Set position by vector components. */
	void setPosition(float x, float y, float z);

	/* Translate by vector. */
	void translate(const Vector3& offset);

	/* Translate by vector components. */
	void translate(float x, float y, float z);

	/* Translate by local direction. */
	void translateLocal(const Vector3& offset);

	/* Translate by local direction. */
	void translateLocal(float x, float y, float z);

	/* Get position vector. */
	Vector3 getTranslation() const;

	/* Get position vector in 2D. */
	Vector2 getTranslation2D() const;

	/* Rotation using quaternion. */
	void rotateQuat(const Quaternion& quaternion);

	/*
	 * Rotation using a normal and angle.
	 * @param axis Axis of rotation.
	 * @param angle Angle in radians.
	 */
	void rotate(const Vector3& axis, float angle);

	/*
	 * Rotation using normal components and angle.
	 * @param x X-component of rotation axis.
	 * @param y Y-component of rotation axis.
	 * @param z Z-component of rotation axis.
	 * @param angle Angle in radians.
	 */
	void rotate(float x, float y, float z, float angle);
	
	/* Rotation using pitch yaw and roll. */
	void rotate(const Vector3& pitchYawRollVector);

	/*
	 * Rotation using pitch yaw and roll.
	 * @param x pitch.
	 * @param y yaw.
	 * @param z roll.
	 */
	void rotate(float pitch, float yaw, float roll);

	/* Rotation using a rotation matrix. */
	void rotate(const XMMATRIX& matrix);

	/* Set rotation to quaternion identity. */
	void setRotationZero();

	/* Set rotation to quaternion. */
	void setRotationQuat(const Quaternion& quaternion);

	/*
	 * Set rotation to axis rotation.
	 * @param axis Axis of rotation.
	 * @param angle Angle in radians.
	 */
	void setRotation(const Vector3& axis, float angle);

	/*
	 * Set rotation to axis rotation.
	 * @param x X-component of axis.
	 * @param y Y-component of axis.
	 * @param z Z-component of axis.
	 * @param angle Angle in radians.
	 */
	void setRotation(float x, float y, float z, float angle);

	/* Rotation using pitch yaw and roll. */
	void setRotation(const Vector3& pitchYawRollVector);

	/*
	 * Set rotation using pitch yaw and roll.
	 * @param x pitch.
	 * @param y yaw.
	 * @param z roll.
	 */
	void setRotation(float pitch, float yaw, float roll);

	/* Rotation using a rotation matrix. */
	void setRotation(const XMMATRIX& matrix);

	/* Set rotation by target position. */
	void lookAt(const Vector3& target);

	/* Set rotation by target position and rotation offset. */
	void lookAt(const Vector3& target, const Quaternion& rotationOffset);

	/* Get rotation quaternion. */
	Quaternion getRotation() const;

	/* Multiply scale by vector. */
	void scale(const Vector3& vector);

	/* Multiply scale by vector components. */
	void scale(float x, float y, float z);

	/* Multiply scale by a single float on all axiss. */
	void scaleUniform(float scale);

	/* Set scale to identity. */
	void setScaleOne();

	/* Set scale to vector components. */
	void setScale(float x, float y, float z);

	/** Set scale to vector. */
	void setScale(const Vector3& scale);

	/* Set scale to a new uniform scale. */
	void setScaleUniform(float scale);

	/* Get scale vector. */
	Vector3 getScaling() const;

	/* Returns a local vector relative to transform. */
	Vector3 getLocalVector(const Vector3& vector) const;

	/* Returns a local vector relative to transform. */
	Vector3 getLocalVector(float x, float y, float z) const;

public: // Macros
	/* Get local forward vector. */
	Vector3 getForwardVector() const;

	/* Get local backward vector. */
	Vector3 getBackwardVector() const;

	/* Get local up vector. */
	Vector3 getUpVector() const;

	/* Get local down vector. */
	Vector3 getDownVector() const;

	/* Get local right vector. */
	Vector3 getRightVector() const;

	/* Get local left vector. */
	Vector3 getLeftVector() const;
};