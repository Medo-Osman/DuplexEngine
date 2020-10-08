#include "3DPCH.h"
#include "Player.h"


void Player::setStates(std::vector<State> states)
{
	m_movementVector = XMVECTOR();
	for (std::vector<int>::size_type i = 0; i < states.size(); i++)
	{
		switch (states[i])
		{
		case WALK_LEFT:
			m_movementVector += XMVectorSet(-2, 0, 0, 0);
			break;
		case WALK_RIGHT:
			m_movementVector += XMVectorSet(2, 0, 0, 0);
			break;
		case WALK_FORWARD:
			m_movementVector += XMVectorSet(0, 0, 2, 0);
			break;
		case WALK_BACKWARD:
			m_movementVector += XMVectorSet(0, 0, -2, 0);
			break;
		default:
			break;
		}
	}
}

Player::Player()
{
	m_movementVector = XMVectorZero();
}

//To do: Implement with physics objects.
void Player::updatePlayer(const float& dt)
{
	m_physicsComponent->addForce(m_movementVector * dt *10);
}

void Player::setPlayerEntity(Entity* entity)
{
	m_playerEntity = entity;
	m_physicsComponent = static_cast<PhysicsComponent*>(m_playerEntity->getComponent("physics"));

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
