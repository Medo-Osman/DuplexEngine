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

void Camera::initialize(const int& fov, const float& aspectRatio, const float& nearZ, const float& farZ)
{
	ApplicationLayer::getInstance().m_input.Attach(this);
	setProjectionMatrix(fov, aspectRatio, nearZ, farZ);
}

void Camera::setProjectionMatrix(const int& fov, const float& aspectRatio, const float& nearZ, const float& farZ)
{
	m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH((fov / 360.f) * DirectX::XM_2PI,
		aspectRatio, nearZ, farZ);
	BoundingFrustum::CreateFromMatrix(m_frustum, m_projectionMatrix);
}
void Camera::setPosition(const Vector3& pos)
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
void Camera::increaseSensitivity()
{
	
	m_sensitivity += 0.01f;
}
void Camera::decreaseSensitivity()
{
	if (m_sensitivity > 0)
	{
		m_sensitivity -= 0.01f;
	}
}
void Camera::increaseFov()
{
	if (fovAmount < 110.0f)
	{
		fovAmount += 1.0f;
	}
}
void Camera::decreaseFov()
{
	if (fovAmount > 60.0f)
	{
		fovAmount -= 1.0f;
	}
	
}
float Camera::getSensitivity()
{
	
	return m_sensitivity * 100;
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
	for (size_t i = 0; i < inputData.rangeData.size(); i++)
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
			rotationF3.x += mouseDelta.y * m_sensitivity;

			// Limit pitch to straight up or straight down with a little fudge-factor to avoid gimbal lock
			float limit = XM_PI / 2.0f - 0.01f;
			rotationF3.x = max(-limit, rotationF3.x);
			rotationF3.x = min(limit, rotationF3.x);

			// Set Yaw
			rotationF3.y += mouseDelta.x * m_sensitivity;

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

void Camera::update(const float& dt)
{
	if (m_isPlayerCamera)
	{
		Player* ply = Engine::get().getPlayerPtr();
		m_position = ply->getPlayerEntity()->getTranslation() + ply->getCameraOffset() + Vector3(0, 0.5f, -1.f);
		m_transform.setPosition(m_position); // Transform pointer used by 3d positional Audio to get the listener position
		if (ply->getRespawnNextFrame()) // Reset camera rotation after respawn
		{
			m_rotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f);
			m_transform.setRotationQuat(m_rotation);
		}

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
	


}

BoundingFrustum Camera::getFrustum()
{
	return m_frustum;
}

//Private
void Camera::updateViewMatrix()
{
	Player* ply = Engine::get().getPlayerPtr();
	Vector3 hitPos;

	float currentRotationAngleY = XMVectorGetY(m_rotation);
	float currentRotationAngleX = XMVectorGetX(m_rotation);

	XMVECTOR currentRotation = XMQuaternionRotationRollPitchYaw(currentRotationAngleX, currentRotationAngleY, 0);

	if (m_isPlayerCamera)
	{
		XMVECTOR playerPos = Vector3(dynamic_cast<CharacterControllerComponent*>(ply->getPlayerEntity()->getComponent("CCC"))->getFootPosition()) + ply->getCameraOffset() + Vector3(0, 0.5, 0);//->getTranslation();

		playerPos += Vector3(0, 0.5f, 0);
		m_position = playerPos;
		XMVECTOR offsetVector = Vector3(0, 0, 1) * 2;
		offsetVector = XMVector3Rotate(offsetVector, currentRotation);
		m_position -= offsetVector;

		Vector3 fromPlayerToCamera = DirectX::XMVector3Normalize(m_position - playerPos);
		if (Physics::get().castRay(playerPos, fromPlayerToCamera, 2, hitPos))
		{
			m_position = hitPos - fromPlayerToCamera * 0.01f;
		}

		if (XMVector3Equal(m_position, playerPos))
		{
			playerPos.m128_f32[1] += 0.00001f;
		}

		XMMATRIX cameraRotation = XMMatrixRotationRollPitchYawFromVector(m_rotation);
		XMVECTOR up = XMVector3TransformCoord(this->upVector, cameraRotation);
		m_viewMatrix = XMMatrixLookAtLH(m_position, playerPos, up);
	}
	else
	{
		XMMATRIX cameraRotation = XMMatrixRotationRollPitchYawFromVector(m_rotation);

		m_viewMatrix = XMMatrixLookAtLH(m_position, XMVector3TransformCoord(forwardVector, cameraRotation), XMVector3TransformCoord(this->upVector, cameraRotation));

	}
	




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
	m_rotation = Vector3(0, XMConvertToRadians(0), 0);
	XMVECTOR up = XMVector3TransformCoord(this->upVector, cameraRotation);
	m_viewMatrix = XMMatrixLookAtLH(m_position, Vector3(5, 1, 0), up);


}
