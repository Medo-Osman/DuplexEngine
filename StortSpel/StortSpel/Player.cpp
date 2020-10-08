#include "3DPCH.h"
#include "Player.h"


Player::Player()
{
	m_movementVector = XMVectorZero();
}

void Player::setStates(std::vector<State> states)
{
	m_movementVector = XMVECTOR();
	for (std::vector<int>::size_type i = 0; i < states.size(); i++)
	{
		switch (states[i])
		{
		case WALK_LEFT:
			m_movementVector += m_cameraTransform->getRightVector() * -1;
			break;
		case WALK_RIGHT:
			m_movementVector += m_cameraTransform->getRightVector();
			break;
		case WALK_FORWARD:
			m_movementVector += m_cameraTransform->getForwardVector();
			break;
		case WALK_BACKWARD:
			m_movementVector += m_cameraTransform->getBackwardVector();
			break;
		default:
			break;
		}
	}
}

float lerp(float a, float b, float t)
{
	return a + (t * (b - a));
}

//To do: Implement with physics objects.
void Player::updatePlayer(const float& dt)
{
	m_physicsComponent->addForce(Vector3(XMVectorGetX(m_movementVector), 0, XMVectorGetZ(m_movementVector)) * dt * m_playerSpeed * 2);

	m_movementVector = XMVector3Normalize(m_movementVector);

	if(Vector3(m_movementVector).LengthSquared() > 0) //Only update when moving
		angleY = XMVectorGetY(XMVector3AngleBetweenNormals(XMVectorSet(0,0,1,0), m_movementVector));

	//if this vector has posisitv value the character is facing the positiv x axis, fixed to check against z axis and not camera forward
	if (XMVectorGetY(XMVector3Cross(m_movementVector, XMVectorSet(0,0,1,0))) > 0.0f)
	{
		angleY = -angleY;
	}

	//This is the current rotation in quaternions
	Quaternion currentRotation = m_playerEntity->getQuaternion();
	currentRotation.Normalize();

	//This is the angleY target quaternion
	Quaternion targetRot = Quaternion::CreateFromYawPitchRoll(angleY, 0, 0);
	targetRot.Normalize();

	//Land somewhere in between target and current
	Quaternion slerped = Quaternion::Slerp(currentRotation, targetRot, dt/0.05);
	slerped.Normalize();

	//Display slerped result
	m_playerEntity->setQuaternion(slerped);
}

void Player::setPlayerEntity(Entity* entity)
{
	m_playerEntity = entity;
	m_physicsComponent = static_cast<PhysicsComponent*>(m_playerEntity->getComponent("physics"));

}

void Player::setCameraTranformPtr(Transform* transform)
{
	m_cameraTransform = transform;
}

Entity* Player::getPlayerEntity()
{
	return m_playerEntity;
}

void Player::inputUpdate(InputData& inputData)
{
	this->setStates(inputData.stateData);
	
	for (std::vector<int>::size_type i = 0; i < inputData.actionData.size(); i++)
	{
		if (inputData.actionData[i] == Action::JUMP)
		{
			m_physicsComponent->addForce(XMFLOAT3(0.f, 10.f, 0.f));
		}
	}

}
