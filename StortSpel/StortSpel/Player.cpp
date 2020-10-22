#include "3DPCH.h"
#include "Player.h"
#include"Pickup.h"
#include"SpeedPickup.h"

Player::Player()
{
	m_movementVector = XMVectorZero();
	m_jumps = 0;
	m_currentDistance = 0;
	m_hasDashed = false;
	m_angleY = 0;
	m_playerEntity = nullptr;
	m_cameraTransform = nullptr;
	m_controller = nullptr;
	m_state = PlayerState::IDLE;

	Physics::get().Attach(this, true, false);
	m_currentSpeedModifier = 1.f;
	m_speedModifierTime = 0;
	if (!Pickup::hasInitPickupArray())
	{
		std::vector<Pickup*> vec;
		vec.emplace_back(new SpeedPickup());
		Pickup::initPickupArray(vec);
	}

	//GUI
	m_score = 0;
	GUITextStyle style;
	style.position.y = 70.f;
	style.scale = { 0.5f };
	m_scoreLabelGUIIndex = GUIHandler::get().addGUIText("Score: ", L"squirk.spritefont", style);
	style.position.x = 160.f;
	style.color = Colors::Yellow;
	m_scoreGUIIndex = GUIHandler::get().addGUIText(std::to_string(m_score), L"squirk.spritefont", style);

}

void Player::setStates(std::vector<State> states)
{
	m_movementVector = XMVECTOR();
	for (std::vector<int>::size_type i = 0; i < states.size(); i++)
	{
		switch (states[i])
		{
		case WALK_LEFT:
			m_movementVector += m_cameraTransform->getLeftVector();
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

void Player::handleRotation(const float &dt)
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

float lerp(const float& a, const float &b, const float &t)
{
	return a + (t * (b - a));
}

void Player::playerStateLogic(const float& dt)
{
	Vector3 finalMovement = XMVector3Normalize(Vector3(XMVectorGetX(m_movementVector), 0, XMVectorGetZ(m_movementVector))) * PLAYER_SPEED  * m_currentSpeedModifier * dt;

	switch (m_state)
	{
	case PlayerState::ROLL:
		if (m_currentDistance >= ROLL_TRAVEL_DISTANCE)
		{
			m_state = PlayerState::IDLE;
			m_controller->setControllerSize(CAPSULE_HEIGHT);
			m_controller->setControllerRadius(CAPSULE_RADIUS);
		}
		else
		{
			m_currentDistance += ROLL_SPEED * dt;
			Vector3 move = m_moveDirection * ROLL_SPEED * dt;
			move.y = -GRAVITY * dt;
			m_controller->move(move, dt);
		}
		break;
	case PlayerState::DASH:
		if (m_currentDistance >= DASH_TRAVEL_DISTANCE)
		{
			m_state = PlayerState::FALLING;
			m_hasDashed = true;
		}
		else
		{
			m_currentDistance += DASH_TRAVEL_DISTANCE * DASH_SPEED * dt;
			m_controller->move(m_moveDirection * DASH_SPEED * DASH_TRAVEL_DISTANCE * dt, dt);
		}
		break;
	case PlayerState::FALLING:
		if (m_controller->checkGround(m_controller->getFootPosition(), Vector3(0.f, -1.f, 0.f), 1.f))
		{
			m_state = PlayerState::IDLE;
			m_jumps = 0;
			m_hasDashed = false;
		}
		else
		{
			finalMovement.y = -JUMP_SPEED * FALL_MULTIPLIER * dt;
			m_controller->move(finalMovement, dt);
		}
		break;
	case PlayerState::JUMPING:
		if (m_currentDistance >= JUMP_DISTANCE)
		{
			m_state = PlayerState::FALLING;
		}
		else
		{
			m_currentDistance += JUMP_SPEED * dt;
			finalMovement.y = JUMP_SPEED * dt;
		}
		m_controller->move(finalMovement, dt);

		break;
	case PlayerState::IDLE:
		finalMovement.y = -GRAVITY * dt;
		m_controller->move(finalMovement, dt);
		break;
	default:
		break;
	}
}

void Player::updatePlayer(const float& dt)
{
	if (m_pickupPointer)
	{
		switch (m_pickupPointer->getPickupType())
		{
		case PickupType::SPEED:
			m_speedModifierTime += dt;
			if (m_speedModifierTime <= FOR_FULL_EFFECT_TIME)
			{
				m_currentSpeedModifier += m_goalSpeedModifier * dt / FOR_FULL_EFFECT_TIME;
			}
			m_pickupPointer->update(dt);
			if(m_pickupPointer->isDepleted())
			{
				if (m_goalSpeedModifier > 0.f)
				{
					m_goalSpeedModifier *= -1;;
					m_speedModifierTime = 0.f;
				}
				
				if (m_speedModifierTime <= FOR_FULL_EFFECT_TIME)
				{
					m_currentSpeedModifier += m_goalSpeedModifier * dt / FOR_FULL_EFFECT_TIME;
					m_currentSpeedModifier = max(1.0f, m_currentSpeedModifier);
				}

				if(m_pickupPointer->shouldDestroy())
				{
					m_pickupPointer->onRemove();
					m_pickupPointer = nullptr;
					m_currentSpeedModifier = 1.f;
				}
			}
			break;
		case PickupType::SCORE:

			break;
		default:
			break;
		}
	}


	if(m_state != PlayerState::ROLL)
		handleRotation(dt);

	playerStateLogic(dt);
}

void Player::setPlayerEntity(Entity* entity)
{
	m_playerEntity = entity;
	m_controller = static_cast<CharacterControllerComponent*>(m_playerEntity->getComponent("CCC"));
	entity->addComponent("ScoreAudio", m_audioComponent = new AudioComponent(m_scoreSound));
}

void Player::setCameraTranformPtr(Transform* transform)
{
	m_cameraTransform = transform;
}

void Player::incrementScore()
{
	m_score++;
	GUIHandler::get().changeGUIText(m_scoreGUIIndex, std::to_string(m_score));
}

void Player::increaseScoreBy(int value)
{
	m_score += value;
	GUIHandler::get().changeGUIText(m_scoreGUIIndex, std::to_string(m_score));
}

int Player::getScore()
{
	return m_score;
}

Entity* Player::getPlayerEntity() const
{
	return m_playerEntity;
}

void Player::inputUpdate(InputData& inputData)
{
	this->setStates(inputData.stateData);
	
	for (std::vector<int>::size_type i = 0; i < inputData.actionData.size(); i++)
	{
		switch (inputData.actionData[i])
		{
		case JUMP:
			if (m_state == PlayerState::IDLE || m_state == PlayerState::JUMPING || m_state == PlayerState::FALLING)
			{
				if (m_state == PlayerState::IDLE)
				{
					jump();
				}
				else
				{
					if (m_jumps < ALLOWED_NR_OF_JUMPS)
					{
						jump();
					}
				}
			}
			break;
		case DASH:
			if (canDash())
				dash();
			break;
		case ROLL:
			if (canRoll())
				roll();
			break;
		case USE:
			incrementScore();

			break;
		default:
			break;
		}
	}
}

void Player::sendPhysicsMessage(PhysicsData& physicsData, bool &shouldTriggerEntityBeRemoved)
{
	if (!shouldTriggerEntityBeRemoved)
	{
		if (physicsData.triggerType == TriggerType::PICKUP)
		{
			if (m_pickupPointer == nullptr)
			{
				bool addPickupByAssosiatedID = true; // If we do not want to add pickup change this to false in switchCase.
				int duration = physicsData.intData;
				switch ((PickupType)physicsData.assosiatedTriggerEnum)
				{
				case PickupType::SPEED:
					m_currentSpeedModifier = 1.f;
					m_goalSpeedModifier = physicsData.floatData;
					m_speedModifierTime = 0;
					shouldTriggerEntityBeRemoved = true;
					break;
				case PickupType::SCORE:
					addPickupByAssosiatedID = false;
					break;
				default:
					break;
				}
				if (addPickupByAssosiatedID)
				{
					m_pickupPointer = Pickup::getPickupByID(physicsData.assosiatedTriggerEnum);
					m_pickupPointer->onPickup(m_playerEntity, duration);
				}
			}
			
			if((PickupType)physicsData.assosiatedTriggerEnum == PickupType::SCORE)
			{
				int amount = (int)physicsData.floatData;
				this->increaseScoreBy(amount);
				m_audioComponent->playSound();
				shouldTriggerEntityBeRemoved = true;
			}

		}
		
	}

}

void Player::jump()
{
	m_currentDistance = 0;
	m_state = PlayerState::JUMPING;
	m_jumps++;
}

bool Player::canRoll() const
{
	return (m_state == PlayerState::IDLE);
}

void Player::roll()
{
	prepDistVariables();
	m_controller->setControllerSize(ROLL_HEIGHT);
	m_controller->setControllerRadius(ROLL_RADIUS);
	m_state = PlayerState::ROLL;
}

bool Player::canDash() const
{
	return (m_state == PlayerState::JUMPING || m_state == PlayerState::FALLING && !m_hasDashed);
}

void Player::dash()
{
	prepDistVariables();
	m_state = PlayerState::DASH;
}

void Player::prepDistVariables()
{
	m_currentDistance = 0;
	m_moveDirection = m_playerEntity->getForwardVector();
}