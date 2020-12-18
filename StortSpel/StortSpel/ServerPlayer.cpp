#include "3DPCH.h"
#include "ServerPlayer.h"


ServerPlayer::ServerPlayer(int ID)
{
	m_playerID = ID;
	m_movementVector = XMVectorZero();
	m_jumps = 0;
	m_currentDistance = 0;
	m_hasDashed = false;
	m_angleY = 0;
	m_playerEntity = nullptr;
	m_cameraTransform = nullptr;
}


void ServerPlayer::handleRotation(const float& dt)
{
	Quaternion currentRotation;
	Quaternion slerped;
	Quaternion targetRot;

	m_movementVector = XMVector3Normalize(m_movementVector);

	if (Vector3(m_movementVector).LengthSquared() > 0) //Only update when moving
		m_angleY = XMVectorGetY(XMVector3AngleBetweenNormals(XMVectorSet(0, 0, 1, 0), m_movementVector));

	//if this vector has posisitv value the character is facing the positiv x axis, fixed to check against z axis and not camera forward
	if (XMVectorGetY(XMVector3Cross(m_movementVector, XMVectorSet(0, 0, 1, 0))) > 0.0f)
	{
		m_angleY = -m_angleY;
	}

	//This is the current rotation in quaternions
	currentRotation = m_playerEntity->getRotation();


	currentRotation.Normalize();

	//This is the angleY target quaternion
	targetRot = Quaternion::CreateFromYawPitchRoll(m_angleY, 0, 0);
	targetRot.Normalize();

	//Land somewhere in between target and current
	slerped = Quaternion::Slerp(currentRotation, targetRot, dt / 0.05f);
	slerped.Normalize();

	//Display slerped result
	m_playerEntity->setRotationQuat(slerped);
}


void ServerPlayer::playerStateLogic(const float& dt)
{
	Vector3 finalMovement = XMVector3Normalize(Vector3(XMVectorGetX(m_movementVector), 0, XMVectorGetZ(m_movementVector))) * PLAYER_SPEED * dt;

}


void ServerPlayer::setPlayerEntity(Entity* entity)
{
	m_playerEntity = entity;
	
}

void ServerPlayer::setCameraTranformPtr(Transform* transform)
{
	m_cameraTransform = transform;
}

Entity* ServerPlayer::getPlayerEntity() const
{
	return m_playerEntity;
}




void ServerPlayer::prepDistVariables()
{
	m_currentDistance = 0;
	m_moveDirection = m_playerEntity->getForwardVector();
}