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
	m_playerEntity->move(m_movementVector * dt);
}

void Player::setPlayerEntity(Entity* entity)
{
	m_playerEntity = entity;
}

Entity* Player::getPlayerEntity()
{
	return m_playerEntity;
}

void Player::inputUpdate(InputData& inputData)
{
	this->setStates(inputData.stateData);
}
