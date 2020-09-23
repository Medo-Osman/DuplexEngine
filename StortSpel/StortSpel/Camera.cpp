#include"3DPCH.h"
#include "Engine.h"
#include"Camera.h"

Camera::Camera()
{
	//m_position = { 0.f, 0.f, 0.f, 1.f };
	m_rotation = { 0.f, 0.f, 0.f, 0.f };
	m_projectionMatrix = XMMatrixIdentity();
	m_viewMatrix = XMMatrixIdentity();
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



void Camera::controllCameraRotation(const MouseEvent& mEvent, const float &dt)
{
	if (mEvent.getEvent() == Event::MouseRAW_MOVE)
	{
		mEvent.getPos();
		m_rotation += {(float)mEvent.getPos().y * dt * 2, (float)mEvent.getPos().x * dt * 2, 0.0f, 0.0f};

		this->updateViewMatrix();
	}
}

void Camera::controllCameraPosition()
{
	
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
	XMMATRIX cameraRotation = XMMatrixRotationRollPitchYawFromVector(m_rotation);
	XMVECTOR cameraLookAt = XMVector3TransformCoord(this->forwardVector, cameraRotation);
	cameraLookAt += m_position;

	XMVECTOR up = XMVector3TransformCoord(this->upVector, cameraRotation);

	//Build view matrix for left-handed coordinate system.
	m_viewMatrix = XMMatrixLookAtLH(m_position, cameraLookAt, up);

	this->curForward = XMVector3TransformCoord(this->forwardVector, cameraRotation);
	this->curUp = XMVector3TransformCoord(this->upVector, cameraRotation);
	this->curRight = XMVector3TransformCoord(this->rightVector, cameraRotation);
}
