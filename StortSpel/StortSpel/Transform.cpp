#include "3DPCH.h"
#include "Transform.h"


Transform::Transform()
	: m_transform(XMMatrixIdentity())

	, m_translation(Vector3(0.0f))
	, m_translationDirty(true)
	, m_translationMat(XMMatrixIdentity())

	, m_rotation(XMQuaternionIdentity())
	, m_rotationDirty(true)
	, m_rotationMat(XMMatrixIdentity())

	, m_scale(Vector3(1.0f))
	, m_scaleDirty(true)
	, m_scaleMat(XMMatrixIdentity())
{}

Transform::~Transform() {}

XMMATRIX Transform::calculateWorldMatrix()
{
	bool dirty = (m_translationDirty || m_scaleDirty || m_rotationDirty);

	// Update translation matrix if dirty
	if (m_translationDirty)
	{
		// Update matrix
		m_translationMat = XMMatrixTranslationFromVector(m_translation);

		// Set dirty state
		m_translationDirty = false;
	}

	// Update scaling matrix if dirty
	if (m_scaleDirty)
	{
		// Update matrix
		m_scaleMat = XMMatrixScalingFromVector(m_scale);

		// Set dirty state
		m_scaleDirty = false;
	}

	// Update translation matrix if dirty
	if (m_rotationDirty)
	{
		// Update matrix
		m_rotationMat = XMMatrixRotationQuaternion(m_rotation);

		// Set dirty state
		m_rotationDirty = false;
	}

	// Only recalculate matrix if dirty
	if (dirty)
	{
		// Generate transformation matrix
		m_transform =  m_scaleMat * m_rotationMat * m_translationMat;
	}

	return m_transform;
}

XMMATRIX Transform::calculateWorldMatrix(XMMATRIX mat)
{
	// TODO Assert order
	return mat * calculateWorldMatrix();
}

XMMATRIX Transform::getTranslationMatrix()
{
	if (m_translationDirty)
	{
		// Update matrix
		m_translationMat = XMMatrixTranslationFromVector(m_translation);

		// Set dirty state
		m_translationDirty = false;
	}

	return m_translationMat;
}

XMMATRIX Transform::getRotationMatrix()
{
	if (m_rotationDirty)
	{
		// Update matrix
		m_rotationMat = XMMatrixRotationQuaternion(m_rotation);

		// Set dirty state
		m_rotationDirty = false;
	}
	return m_rotationMat;
}

XMMATRIX Transform::getScaleMatrix()
{
	if (m_scaleDirty)
	{
		// Update matrix
		m_scaleMat = XMMatrixScalingFromVector(m_scale);

		// Set dirty state
		m_scaleDirty = false;
	}
	return m_scaleMat;
}

void Transform::setPositionZero()
{
	if (m_translation != Vector3(0.0f))
	{
		m_translation = Vector3(0.0f);
		m_translationDirty = true;
	}
}

void Transform::setPosition(const Vector3& position)
{
	if (m_translation != position)
	{
		m_translation = position;
		m_translationDirty = true;
	}
}

void Transform::setPosition(float x, float y, float z)
{
	Vector3 v(x, y, z);
	if (m_translation != v)
	{
		m_translation = v;
		m_translationDirty = true;
	}
}

void Transform::translate(const Vector3& offset)
{
	m_translation += offset;
	m_translationDirty = true;
}

void Transform::translate(float x, float y, float z)
{
	m_translation += Vector3(x, y, z);
	m_translationDirty = true;
}

void Transform::translateLocal(const Vector3& offset)
{
	m_translation += getLocalVector(offset);
	m_translationDirty = true;
}

void Transform::translateLocal(float x, float y, float z)
{
	m_translation += getLocalVector(x, y, z);
	m_translationDirty = true;
}

Vector3 Transform::getTranslation() const
{
	return m_translation;
}

Vector2 Transform::getTranslation2D() const
{
	return Vector2(m_translation.x, m_translation.z);
}

void Transform::rotateQuat(const Quaternion& quaternion)
{
	m_rotation *= quaternion;
	m_rotation.Normalize();
	m_rotationDirty = true;
}

void Transform::rotate(const Vector3& axis, float angle)
{
	m_rotation = XMQuaternionMultiply(m_rotation, XMQuaternionRotationAxis(axis, angle));

	m_rotationDirty = true;
}

void Transform::rotate(float x, float y, float z, float angle)
{
	m_rotation = XMQuaternionMultiply(m_rotation, XMQuaternionRotationAxis(Vector3(x, y, z), angle));
	m_rotationDirty = true;
}

void Transform::rotate(const Vector3& pitchYawRollVector)
{
	rotateQuat( Quaternion::CreateFromYawPitchRoll(pitchYawRollVector.y, pitchYawRollVector.x, pitchYawRollVector.z) );
}

void Transform::rotate(float pitch, float yaw, float roll)
{
	rotateQuat( Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll) );
}

void Transform::rotate(const XMMATRIX& matrix)
{
	rotateQuat( Quaternion(XMQuaternionRotationMatrix(matrix)) );
}

void Transform::setRotationZero()
{
	m_rotation = XMQuaternionIdentity();
	m_rotationDirty = true;
}

void Transform::setRotationQuat(const Quaternion& quaternion)
{
	if (m_rotation != quaternion)
	{
		m_rotation = quaternion;
		m_rotation.Normalize();
		m_rotationDirty = true;
	}
}

void Transform::setRotation(const Vector3& axis, float angle)
{
	Quaternion q = XMQuaternionRotationAxis(axis, angle);
	if (m_rotation != q)
	{
		m_rotation = q;
		m_rotationDirty = true;
	}
}

void Transform::setRotation(float x, float y, float z, float angle)
{
	Quaternion q = XMQuaternionRotationAxis(Vector3(x, y, z), angle);
	if (m_rotation != q)
	{
		m_rotation = q;
	}
	m_rotationDirty = true;
}

void Transform::setRotation(const Vector3& pitchYawRollVector)
{
	setRotationQuat( Quaternion::CreateFromYawPitchRoll(pitchYawRollVector.y, pitchYawRollVector.x, pitchYawRollVector.z) );
}

void Transform::setRotation(float pitch, float yaw, float roll)
{
	setRotationQuat( Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll) );
}

void Transform::setRotation(const XMMATRIX& matrix)
{
	setRotationQuat(XMQuaternionRotationMatrix(matrix));
}

void Transform::lookAt(const Vector3& target)
{
	Vector3 dir = XMVector3Normalize(target - getTranslation());

	float angleY = std::atan2(dir.z, dir.x) - (XM_PI / 2.0f);
	float angleX = XMVectorGetX(XMVector3Dot(dir, (Vector3(0.0f, 1.0f, 0.0f)))) * XM_PI * 0.5f;

	// Rotate horizontal (Y Axis)
	setRotation(Vector3(0.0f, 1.0f, 0.0f), angleY);

	// Rotate vertical (X Axis)
	rotate(Vector3(1.0f, 0.0f, 0.0f), angleX);

	m_rotationDirty = true;
}

void Transform::lookAt(const Vector3& target, const Quaternion& rotationOffset)
{
	Vector3 dir = XMVector3Normalize(target - getTranslation());
	float angleY = std::atan2(dir.z, dir.x) - (XM_PI / 2.0f);
	float angleX = XMVectorGetX(XMVector3Dot(dir, (Vector3(0.0f, 1.0f, 0.0f)))) * XM_PI * 0.5f;

	// Set rotation to `rotationOffset`
	setRotationQuat(rotationOffset);

	// Rotate horizontal (Y Axis)
	rotate(Vector3(0.0f, 1.0f, 0.0f), angleY);

	// Rotate vertical (X Axis)
	rotate(Vector3(1.0f, 0.0f, 0.0f), angleX);

	m_rotationDirty = true;
}

Quaternion Transform::getRotation() const
{
	return m_rotation;
}

void Transform::scale(const Vector3& vector)
{
	m_scale *= vector;
	m_scaleDirty = true;
}

void Transform::scale(float x, float y, float z)
{
	m_scale *= Vector3(x, y, z);
	m_scaleDirty = true;
}

void Transform::scaleUniform(float scale)
{
	this->scale(scale, scale, scale);
}

void Transform::setScaleOne()
{
	m_scale = Vector3(1.0f, 1.0f, 1.0f);
	m_scaleDirty = true;
}

void Transform::setScale(float x, float y, float z)
{
	Vector3 s = Vector3(x, y, z);
	if (m_scale != s)
	{
		m_scale = s;
		m_scaleDirty = true;
	}
}

void Transform::setScale(const Vector3& scale)
{
	if (m_scale != scale)
	{
		m_scale = scale;
		m_scaleDirty = true;
	}
}

void Transform::setScaleUniform(float scale)
{
	setScale(scale, scale, scale);
}

Vector3 Transform::getScaling() const
{
	return m_scale;
}

Vector3 Transform::getLocalVector(const Vector3& vector) const
{
	return XMVector3Rotate(vector, XMQuaternionInverse(m_rotation));
	return XMVector3Rotate(vector, m_rotation);
}

Vector3 Transform::getLocalVector(float x, float y, float z) const
{
	//return XMVector3Rotate(Vector3(x, y, z), XMQuaternionInverse(m_rotation)); TODO: ask if this is supposed to be inversed
	return XMVector3Rotate(Vector3(x, y, z), m_rotation);
}

Vector3 Transform::getForwardVector() const {
	return getLocalVector(0.0f, 0.0f, 1.0f);
}

Vector3 Transform::getBackwardVector() const {
	return getLocalVector(0.0f, 0.0f, -1.0f);
}

Vector3 Transform::getUpVector() const {
	return getLocalVector(0.0f, 1.0f, 0.0f);
}

Vector3 Transform::getDownVector() const {
	return getLocalVector(0.0f, -1.0f, 0.0f);
}

Vector3 Transform::getRightVector() const {
	return getLocalVector(1.0f, 0.0f, 0.0f);
}

Vector3 Transform::getLeftVector() const {
	return getLocalVector(-1.0f, 0.0f, 0.0f);
}