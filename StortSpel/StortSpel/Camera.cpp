#include"3DPCH.h"
#include "Engine.h"
#include"Camera.h"
#include"ApplicationLayer.h"

Camera::Camera()
{
	//m_position = { 0.f, 0.f, 0.f, 1.f };
	m_rotation = { 0.f, 0.f, 0.f, 0.f };
	m_projectionMatrix = XMMatrixIdentity();
	m_viewMatrix = XMMatrixIdentity();
	m_newIncrements = false;
	ApplicationLayer::getInstance().m_input.Attach(this);
}
void Camera::setProjectionMatrix(const float& fov, const float& aspectRatio, const float& nearZ, const float& farZ)
{
	m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH((fov / 360.f) * DirectX::XM_2PI,
		aspectRatio, nearZ, farZ);
}
void Camera::setPosition(const XMVECTOR& pos)
{
	m_position = pos;
	this->updateViewMatrix();
}
const XMVECTOR& Camera::getPosition() const
{
	return m_position;
}
const XMFLOAT3 Camera::getFloat3Position() const
{
	XMFLOAT3 floatPos;
	XMStoreFloat3(&floatPos, m_position);
	return floatPos;
}
void Camera::setRotation(const XMVECTOR& rot)
{
	m_rotation = rot;
	this->updateViewMatrix();
}
const XMVECTOR& Camera::getRotation() const
{
	return m_rotation;
} 
const XMFLOAT4 Camera::getFloat4Rotation() const
{
	XMFLOAT4 float4Rot;
	XMStoreFloat4(&float4Rot, XMQuaternionRotationRollPitchYawFromVector(m_rotation));
	return float4Rot;
}

const XMMATRIX& Camera::getViewMatrix() const
{
	return m_viewMatrix;
}
const XMMATRIX& Camera::getProjectionMatrix() const
{
	return m_projectionMatrix;
}

void Camera::inputUpdate(InputData& inputData)
{
	for (std::vector<int>::size_type i = 0; i < inputData.rangeData.size(); i++)
	{
		if (inputData.rangeData[i].rangeFlag == Range::RAW)
		{
			if (!m_newIncrements)
			{
				m_incrementRotation = XMVectorZero();
				m_newIncrements = true;
			}
			m_incrementRotation += {(float)inputData.rangeData[i].pos.y, (float)inputData.rangeData[i].pos.x};
		}
	}
}

void Camera::update(const float &dt)
{
	if (m_newIncrements)
	{
		this->m_rotation += m_incrementRotation * dt * 2;
		m_newIncrements = false;
	}
	m_position = Engine::get().getEntity("meshPlayer")->getTranslation() + Vector3(0, 2, -5);

	this->updateViewMatrix();
}

const XMFLOAT3 Camera::getForward() const
{
	XMFLOAT3 forward;
	XMStoreFloat3(&forward, this->curForward);
	return forward;
}

BoundingFrustum Camera::getFrustum()
{
	BoundingFrustum frust;
	BoundingFrustum::CreateFromMatrix(frust, m_projectionMatrix);
	return frust;
}


//Private
void Camera::updateViewMatrix()
{
	float currentRotationAngleY = XMVectorGetY(m_rotation);
	float currentRotationAngleX = XMVectorGetX(m_rotation);
	
	XMVECTOR currentRotation = XMQuaternionRotationRollPitchYaw(currentRotationAngleX, currentRotationAngleY, 0);

	XMVECTOR playerPos = Engine::get().getEntity("meshPlayer")->getTranslation();
	playerPos += Vector3(0, 1.75f, 0);
	m_position = playerPos;
	XMVECTOR offsetVector = Vector3(0, 0, 1) * 5;
	offsetVector = XMVector3Rotate(offsetVector, currentRotation);
	m_position -= offsetVector;
	
	XMMATRIX cameraRotation = XMMatrixRotationRollPitchYawFromVector(m_rotation);
	XMVECTOR up = XMVector3TransformCoord(this->upVector, cameraRotation);
	m_viewMatrix = XMMatrixLookAtLH(m_position, playerPos, up);

	this->curForward = XMVector3TransformCoord(this->forwardVector, cameraRotation);
	this->curUp = XMVector3TransformCoord(this->upVector, cameraRotation);
	this->curRight = XMVector3TransformCoord(this->rightVector, cameraRotation);
}
