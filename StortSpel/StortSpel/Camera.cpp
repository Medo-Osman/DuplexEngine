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
}
Camera::~Camera() {}

void Camera::initialize(const float& fov, const float& aspectRatio, const float& nearZ, const float& farZ)
{
	ApplicationLayer::getInstance().m_input.Attach(this);
	setProjectionMatrix(fov, aspectRatio, nearZ, farZ);
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

Transform* Camera::getTransform()
{
	return &m_transform;
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

			XMFLOAT2 mouseDelta = XMFLOAT2(inputData.rangeData[i].pos.x, inputData.rangeData[i].pos.y) * m_sensitivity;

			//(float)inputData.rangeData[i].pos

			// Set Pitch
			XMFLOAT3 rotationF3;
			XMStoreFloat3(&rotationF3, m_rotation);
			rotationF3.x += mouseDelta.y * 0.02f;

			// Limit pitch to straight up or straight down with a little fudge-factor to avoid gimbal lock
			float limit = XM_PI / 2.0f - 0.01f;
			rotationF3.x = max(-limit, rotationF3.x);
			rotationF3.x = min(limit, rotationF3.x);

			// Set Yaw
			rotationF3.y += mouseDelta.x * 0.02f;

			// Keep longitude in sane range by wrapping
			if (rotationF3.x > XM_PI)
				rotationF3.y -= XM_PI * 2.0f;
			else if (rotationF3.x < -XM_PI)
				rotationF3.y += XM_PI * 2.0f;

			m_rotation = XMLoadFloat3(&rotationF3);
			m_transform.setRotation(m_rotation);
		}
	}
}

void Camera::update(const float &dt)
{
	//if (m_newIncrements)
	//{
	//	this->m_rotation += m_incrementRotation * dt * 2;
	//	m_transform.rotate(m_rotation);
	//	m_newIncrements = false;
	//}
	Player* ply = Engine::get().getPlayerPtr();
	m_position = ply->getPlayerEntity()->getTranslation() + ply->getCameraOffset() + Vector3(0, 2, -5);
	m_transform.setPosition(m_position); // Transform pointer used by 3d positional Audio to get the listener position

	//endscene fixed camera position
	if (endSceneCamera)
	{
		frustumCullingOn = false;
		this->updateViewMatrixEndScene();
	}
	else
	{
		frustumCullingOn = true;
		this->updateViewMatrix();
	}
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
	Player* ply = Engine::get().getPlayerPtr();

	float currentRotationAngleY = XMVectorGetY(m_rotation);
	float currentRotationAngleX = XMVectorGetX(m_rotation);
	
	XMVECTOR currentRotation = XMQuaternionRotationRollPitchYaw(currentRotationAngleX, currentRotationAngleY, 0);

	XMVECTOR playerPos = Vector3(dynamic_cast<CharacterControllerComponent*>(ply->getPlayerEntity()->getComponent("CCC"))->getFootPosition()) + ply->getCameraOffset() + Vector3(0, 0.5, 0);//->getTranslation();

	playerPos += Vector3(0, 1.75f, 0);
	m_position = playerPos;
	XMVECTOR offsetVector = Vector3(0, 0, 1) * 5;
	offsetVector = XMVector3Rotate(offsetVector, currentRotation);
	m_position -= offsetVector;

	XMMATRIX cameraRotation = XMMatrixRotationRollPitchYawFromVector(m_rotation);
	XMVECTOR up = XMVector3TransformCoord(this->upVector, cameraRotation);
	m_viewMatrix = XMMatrixLookAtLH(m_position, playerPos, up);

	// = XMVector3TransformCoord(this->forwardVector, cameraRotation);
	//m_curUp = XMVector3TransformCoord(this->upVector, cameraRotation);
	//m_curRight = XMVector3TransformCoord(this->rightVector, cameraRotation);
}

void Camera::updateViewMatrixEndScene()
{

	float currentRotationAngleY = 0;
	float currentRotationAngleX = 0;

	XMVECTOR camPos = Vector3(5, 1, 7);
	m_position = camPos;
	XMMATRIX cameraRotation = XMMatrixRotationRollPitchYawFromVector(XMVECTOR{ 0,1,0 });
	m_rotation = Vector3(0,XMConvertToRadians(0),0);
	XMVECTOR up = XMVector3TransformCoord(this->upVector, cameraRotation);
	m_viewMatrix = XMMatrixLookAtLH(m_position, Vector3(5,1,0), up);

	
}
