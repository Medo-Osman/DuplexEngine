#include "3DPCH.h"
#include "Player.h"


Player::Player()
{
}

//Updates player, should later integrate with new inputhandler.
//To do: Implement with physics objects.
void Player::updatePlayer(Keyboard* keyboardPtr, const float& dt)
{
	m_movementVector = XMVECTOR();
	if (keyboardPtr->isKeyPressed('W')) //W-key
	{
		m_movementVector += XMVectorSet( 0, 0, 2*dt, 0 );
	}
	if (keyboardPtr->isKeyPressed('A')) //A-key
	{
		m_movementVector += XMVectorSet( -2*dt, 0, 0, 0 );
	}
	if (keyboardPtr->isKeyPressed('S')) //S-key
	{
		m_movementVector += XMVectorSet(0, 0, -2*dt, 0);
	}

	if (keyboardPtr->isKeyPressed('D')) //D-key
	{
		m_movementVector += XMVectorSet( 2*dt, 0, 0, 0 );
	}
	
	m_playerEntity->move(m_movementVector);
}

void Player::setPlayerEntity(Entity* entity)
{
	m_playerEntity = entity;
}

Entity* Player::getPlayerEntity()
{
	return m_playerEntity;
}
