#include "3DPCH.h"
#include "Player.h"


Player::Player()
{
	m_movementVector = XMVectorZero();
	m_height = 0;
	m_jumps = 0;
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
	Vector3 finalMovement = XMVector3Normalize(Vector3(XMVectorGetX(m_movementVector), 0, XMVectorGetZ(m_movementVector))) * dt * m_playerSpeed;

	if(m_height >= MAX_FALL_SPEED)
		m_height += GRAVITY_MODIFIER * dt;

	switch (m_state)
	{
	case PlayerState::IDLE:
		break;
	case PlayerState::DO_JUMP:
		m_height = INITAL_JUMP_VELOCITY;
		m_state = PlayerState::JUMPING;
		m_jumps++;
		break;
	case PlayerState::JUMPING:
		if (m_height < 0)
		{
			if (m_controller->checkGround(m_controller->getFootPosition(), Vector3(0.f, -1.f, 0.f), 1.f))
			{
				m_state = PlayerState::IDLE;
				m_jumps = 0;
			}
		}
		break;
	default:
		break;
	}
		

	finalMovement.y = m_height;
	m_controller->move(finalMovement, dt);


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
	m_controller = static_cast<CharacterControllerComponent*>(m_playerEntity->getComponent("CCC"));

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
			if (m_state != PlayerState::JUMPING)
			{
				m_state = PlayerState::DO_JUMP;
			}
			else
			{
				if (m_jumps < 2)
				{
					m_state = PlayerState::DO_JUMP;
				}
			}
			//if (m_controller->checkGround(m_playerEntity->getTranslation() - Vector3(0.f, 1.1f, 0.f), Vector3(0.f, -1.f, 0.f), 1.f))
			//{
			//	
			//}
		}
	}

}
