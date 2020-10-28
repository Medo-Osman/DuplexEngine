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
	m_animMesh = nullptr;
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

	GUIImageStyle imageStyle;
	imageStyle.position = Vector2(400, 150);
	imageStyle.scale = Vector2(1, 1);
	m_instructionGuiIndex = GUIHandler::get().addGUIImage(L"keyboard.png", imageStyle);

}

Player::~Player()
{
	
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

	if (Vector3(m_movementVector).LengthSquared() > 0)
	{
		//This is the current rotation in quaternions
		currentRotation = m_playerEntity->getRotation();
		currentRotation.Normalize();

		auto cameraRot = m_cameraTransform->getRotation();
		auto offset = Vector4(XMVector3AngleBetweenNormals(XMVector3Normalize(m_movementVector), m_cameraTransform->getForwardVector()));
		
		//if this vector has posisitv value the character is facing the positiv x axis, checks movementVec against cameraForward
		if (XMVectorGetY(XMVector3Cross(m_movementVector, m_cameraTransform->getForwardVector())) > 0.0f)
		{
			//m_angleY = -m_angleY;
			offset.y = -offset.y;
		}

		//This is the angleY target quaternion
		targetRot = Quaternion(0, cameraRot.y, 0, cameraRot.w) * Quaternion::CreateFromYawPitchRoll(offset.y, 0, 0);//Quaternion::CreateFromYawPitchRoll(m_angleY, 0, 0);
		targetRot.Normalize();
	
		//Land somewhere in between target and current
		slerped = Quaternion::Slerp(currentRotation, targetRot, dt / 0.08f);
		slerped.Normalize();

		//Display slerped result
		m_playerEntity->setRotationQuat(slerped);

	}
	//m_playerEntity->
}

float lerp(const float& a, const float &b, const float &t)
{
	return a + (t * (b - a));
}

void Player::playerStateLogic(const float& dt)
{

	m_finalMovement = Vector3(XMVector3Normalize(Vector3(XMVectorGetX(m_movementVector), 0, XMVectorGetZ(m_movementVector))) * PLAYER_SPEED * dt * this->m_currentSpeedModifier) + Vector3(0, m_finalMovement.y, 0);

	switch (m_state)
	{
	case PlayerState::ROLL:
		if (m_currentDistance >= ROLL_TRAVEL_DISTANCE)
		{
			m_state = PlayerState::IDLE;
			m_controller->setControllerSize(CAPSULE_HEIGHT);
			m_controller->setControllerRadius(CAPSULE_RADIUS);
			m_animMesh->playAnimation("Running4.1", true);
		}
		else
		{
			m_currentDistance += ROLL_SPEED * dt;
			Vector3 move = m_moveDirection * ROLL_SPEED * dt;
			move.y += -GRAVITY * dt;
			m_finalMovement += move;
			//m_controller->move(move, dt);
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
			m_finalMovement += m_moveDirection * DASH_SPEED * DASH_TRAVEL_DISTANCE * dt;
			//m_controller->move(m_moveDirection * DASH_SPEED * DASH_TRAVEL_DISTANCE * dt, dt);
		}
		break;
	case PlayerState::FALLING:
		if (m_controller->checkGround(m_controller->getFootPosition(), Vector3(0.f, -1.f, 0.f), 1.f))
		{
			m_state = PlayerState::IDLE;
			m_jumps = 0;
			m_finalMovement.y = 0;
			m_hasDashed = false;
		}
		else
		{
			//finalMovement.y += finalMovement.y - 1.f*dt;//-JUMP_SPEED * FALL_MULTIPLIER * dt;
			//m_controller->move(finalMovement, dt);
		}
		break;
	case PlayerState::JUMPING:
		m_finalMovement.y = JUMP_SPEED * dt;// * dt;

		m_currentDistance += JUMP_SPEED * dt;

		if (m_currentDistance > JUMP_DISTANCE)
		{
			m_currentDistance = 0.f;
			m_state = PlayerState::FALLING;
		}

		break;
	case PlayerState::IDLE:

		break;
	default:
		break;
	}

	if (m_finalMovement.y > -MAX_FALL_SPEED * dt)
		m_finalMovement += Vector3(0, -GRAVITY * dt, 0);
	m_controller->move(m_finalMovement, dt);
	
	float vectorLen = Vector3(m_finalMovement.x, 0, m_finalMovement.z).LengthSquared();
	if (m_state != PlayerState::ROLL)
	{
		if (vectorLen > 0)
			m_animMesh->setAnimationSpeed(1);
		else
			m_animMesh->setAnimationSpeed(0);
	}
	

	if (m_controller->getFootPosition().y < (float)m_heightLimitBeforeRespawn)
	{
		respawnPlayer();
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

	ImGui::Begin("Player Information", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
	ImGui::Text("Player Position: (%d %d, %d)", (int)this->getPlayerEntity()->getTranslation().x, (int)this->getPlayerEntity()->getTranslation().y, (int)this->getPlayerEntity()->getTranslation().z);
	ImGui::End();
}

void Player::setPlayerEntity(Entity* entity)
{
	m_playerEntity = entity;
	m_controller = static_cast<CharacterControllerComponent*>(m_playerEntity->getComponent("CCC"));
	entity->addComponent("ScoreAudio", m_audioComponent = new AudioComponent(m_scoreSound));
}

Vector3 Player::getCheckpointPos()
{
	return m_checkpointPos;
}

void Player::setCheckpoint(Vector3 newPosition)
{
	m_checkpointPos = newPosition;
}

void Player::setCameraTranformPtr(Transform* transform)
{
	m_cameraTransform = transform;
}

void Player::setAnimMeshPtr(AnimatedMeshComponent* animatedMesh)
{
	m_animMesh = animatedMesh;
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

void Player::respawnPlayer()
{
	m_controller->setPosition(m_checkpointPos);
}

int Player::getScore()
{
	return m_score;
}

void Player::setScore(int newScore)
{
	m_score = newScore;
	GUIHandler::get().changeGUIText(m_scoreGUIIndex, std::to_string(m_score));

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

			break;
		case CLOSEINTROGUI:
			GUIHandler::get().setVisible(m_instructionGuiIndex, false);
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
		if (physicsData.triggerType == TriggerType::CHECKPOINT)
		{

			Entity* ptr = static_cast<Entity*>(physicsData.pointer);

			CheckpointComponent* checkpointPtr = dynamic_cast<CheckpointComponent*>(ptr->getComponent("checkpoint"));
			if (!checkpointPtr->isUsed())
			{
				AudioComponent* audioPtr = dynamic_cast<AudioComponent*>(ptr->getComponent("sound"));
				audioPtr->playSound();

				m_checkpointPos = ptr->getTranslation();

				checkpointPtr->setUsed(true);
			}


		}

		if (physicsData.triggerType == TriggerType::PICKUP)
		{
			if (m_pickupPointer == nullptr)
			{
				bool addPickupByAssosiatedID = true; // If we do not want to add pickup change this to false in switchCase.
				int duration = physicsData.intData;
				switch ((PickupType)physicsData.associatedTriggerEnum)
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
					m_pickupPointer = Pickup::getPickupByID(physicsData.associatedTriggerEnum);
					m_pickupPointer->onPickup(m_playerEntity, duration);
				}
			}
			
			if((PickupType)physicsData.associatedTriggerEnum == PickupType::SCORE)
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
	m_animMesh->playAnimation("platformer_guy_roll1", true);
	m_animMesh->setAnimationSpeed(0.8f);
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