#include "3DPCH.h"
#include "Player.h"
#include"Pickup.h"
#include"SpeedPickup.h"
#include"ParticleComponent.h"
#include "Traps.h"

Pickup* getCorrectPickupByID(int id);

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
	imageStyle.position = Vector2(400.f, 50.f);
	imageStyle.scale = Vector2(0.9f, 0.9f);
	m_instructionGuiIndex = GUIHandler::get().addGUIImage(L"keyboard.png", imageStyle);

	//Test Button stuff
	GUIButtonStyle btnStyle;
	btnStyle.position = Vector2(240, 200);
	btnStyle.scale = Vector2(0.5, 0.5);
	closeInstructionsBtnIndex = GUIHandler::get().addGUIButton(L"closeButton.png", btnStyle);

	//Attach to the click listener for the button
	dynamic_cast<GUIButton*>(GUIHandler::get().getElementMap()->at(closeInstructionsBtnIndex))->Attach(this);
}

Player::~Player()
{
	if (m_playerEntity)
		delete m_playerEntity;
}

bool Player::isRunning()
{
	Vector3 vec = m_movementVector;
	return (vec.Length() > 0);
}

void Player::setStates(std::vector<State> states)
{
	m_movementVector = Vector3();
	if (m_state != PlayerState::DASH && m_state != PlayerState::ROLL)
	{
		for (size_t i = 0; i < states.size(); i++)
		{
			switch (states[i])
			{
			case WALK_LEFT:		m_movementVector += m_cameraTransform->getLeftVector(); break;
			case WALK_RIGHT:	m_movementVector += m_cameraTransform->getRightVector(); break;
			case WALK_FORWARD:	m_movementVector += m_cameraTransform->getForwardVector(); break;
			case WALK_BACKWARD: m_movementVector += m_cameraTransform->getBackwardVector(); break;	
			default: break;
			}
		}
	}
	m_movementVector.Normalize();
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
		slerped = Quaternion::Slerp(currentRotation, targetRot, dt / PLAYER_ROTATION_SPEED);
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
	Vector3 directionalMovement = Vector3(m_movementVector.x, 0, m_movementVector.z);
	switch (m_state)
	{
	case PlayerState::ROLL:

		if (m_currentDistance >= ROLL_TRAVEL_DISTANCE)
		{
			m_lastState = PlayerState::ROLL;
			m_state = PlayerState::IDLE;
			m_controller->setControllerSize(CAPSULE_HEIGHT);
			m_controller->setControllerRadius(CAPSULE_RADIUS);
			//m_animMesh->playAnimation("Running4.1", true);
			idleAnimation();
		}
		else
		{
			/*Vector3 move = m_moveDirection * ROLL_SPEED * dt;
			move.y += -GRAVITY;*/
			directionalMovement = m_moveDirection;
			m_horizontalMultiplier += ROLL_SPEED * dt;

			m_currentDistance += ROLL_SPEED * dt;
		}
		break;

	case PlayerState::DASH:

		if (m_currentDistance >= DASH_TRAVEL_DISTANCE)
		{
			m_lastState = PlayerState::DASH;
			m_state = PlayerState::FALLING;
			m_hasDashed = true;
			m_horizontalMultiplier = PLAYER_MAX_SPEED;
			m_beginDashSpeed = -1.f;
			idleAnimation();
		}
		else
		{
			/*m_currentDistance += DASH_TRAVEL_DISTANCE * DASH_SPEED;
			m_velocity += m_moveDirection * DASH_SPEED * DASH_TRAVEL_DISTANCE;*/

			m_currentDistance += DASH_SPEED * dt * m_currentSpeedModifier;

			float t = m_currentDistance / (DASH_TRAVEL_DISTANCE * m_currentSpeedModifier);
			float sqt = t * t;
			float parametricBlend = sqt / (2.0f * (sqt - t) + 1.0f);
			float speedValue = lerp(m_beginDashSpeed, DASH_TRAVEL_DISTANCE * m_currentSpeedModifier, parametricBlend); // m_beginDashSpeed is set in dash() function
			m_horizontalMultiplier = speedValue;

			directionalMovement = m_moveDirection;
			//m_horizontalMultiplier += m_currentDistance * DASH_SPEED * dt;
			m_verticalMultiplier = 0.f;
		}
		break;

	case PlayerState::FALLING:

		if (directionalMovement.LengthSquared() > 0)
		{
			// Add Speed
			m_horizontalMultiplier += PLAYER_AIR_ACCELERATION * dt * m_currentSpeedModifier;

			// Limit Speed
			if (m_horizontalMultiplier > PLAYER_MAX_SPEED * m_currentSpeedModifier)
				m_horizontalMultiplier = PLAYER_MAX_SPEED * m_currentSpeedModifier;
		}

		// Limit Jump
		if (m_jumps == 0) // Can only jump once in air
			m_jumps = ALLOWED_NR_OF_JUMPS - 1;

		// On Ground Check
		if (m_controller->checkGround(m_controller->getFootPosition(), Vector3(0.f, -1.f, 0.f), 0.1f))
		{
			m_lastState = PlayerState::FALLING;
			m_state = PlayerState::IDLE;
			m_jumps = 0;
			m_hasDashed = false;
		}
		break;

	case PlayerState::JUMPING:

		if (directionalMovement.LengthSquared() > 0)
		{
			// Add Speed
			m_horizontalMultiplier += PLAYER_AIR_ACCELERATION * dt * m_currentSpeedModifier;

			// Limit Speed
			if (m_horizontalMultiplier > PLAYER_MAX_SPEED * m_currentSpeedModifier)
				m_horizontalMultiplier = PLAYER_MAX_SPEED * m_currentSpeedModifier;
		}

		if (m_verticalMultiplier < 0.f)
		{
			m_currentDistance = 0.f;
			m_lastState = PlayerState::JUMPING;
			m_state = PlayerState::FALLING;
		}

		break;

	case PlayerState::IDLE:

		if (directionalMovement.LengthSquared() > 0)
		{
			// Add Speed
			m_horizontalMultiplier += PLAYER_ACCELERATION * dt * m_currentSpeedModifier;

			// Limit Speed
			if (m_horizontalMultiplier > PLAYER_MAX_SPEED * m_currentSpeedModifier)
				m_horizontalMultiplier = PLAYER_MAX_SPEED * m_currentSpeedModifier;
		}

		if (!m_controller->checkGround(m_controller->getFootPosition(), Vector3(0.f, -1.f, 0.f), 0.1f))
		{
			m_lastState = PlayerState::IDLE;
			m_state = PlayerState::FALLING;
			m_verticalMultiplier = 0.f;
		}
		break;

	default:
		break;
	}

	// Gravity
	if (m_state == PlayerState::FALLING || m_state == PlayerState::JUMPING || m_state == PlayerState::ROLL)
	{
		if (m_playerEntity->getTranslation().y != m_lastPosition.y || m_verticalMultiplier == 0)
			m_verticalMultiplier -= GRAVITY * m_gravityScale * dt;
	}
	/*if (m_controller->checkGround(m_controller->getFootPosition(), Vector3(0.f, -1.f, 0.f), 0.1f) && m_state != PlayerState::JUMPING)
		m_verticalMultiplier = 0.f;*/

	// Max Gravity Tests
	if (m_verticalMultiplier <= -MAX_FALL_SPEED)
		m_verticalMultiplier = -MAX_FALL_SPEED;

	// Multiplier Print
	m_timeCounter += dt;
	if (m_timeCounter > 1.f)
	{
		std::cout << m_horizontalMultiplier << "\n";
		std::cout << m_currentSpeedModifier << "\n";

		switch (m_state)
		{
		case PlayerState::DASH:		std::cout << "DASH\n"; break;
		case PlayerState::ROLL:		std::cout << "ROLL\n"; break;
		case PlayerState::JUMPING:	std::cout << "JUMPING\n"; break;
		case PlayerState::FALLING:	std::cout << "FALLING\n"; break;
		case PlayerState::IDLE:		std::cout << "IDLE\n"; break;
		default: break;
		}

		std::cout << "\n";

		m_timeCounter -= 1.f;
	}

	// Final frame velocity
	if (directionalMovement.LengthSquared() > 0)
		m_lastDirectionalMovement = directionalMovement;
	
	m_velocity = ((m_lastDirectionalMovement * m_horizontalMultiplier) + (Vector3(0, 1, 0) * m_verticalMultiplier)) * dt;

	m_controller->move(m_velocity, 1.f);
	m_lastPosition = m_playerEntity->getTranslation();

	// Animation
	//float vectorLen = Vector3(m_velocity.x, 0, m_velocity.z).LengthSquared();
	float vectorLen = Vector3(m_velocity.x, 0, m_velocity.z).Length();
	if (m_state != PlayerState::ROLL && m_state != PlayerState::DASH)
	{
		float blend = vectorLen / (PLAYER_MAX_SPEED * dt);
		
		if ((PLAYER_MAX_SPEED * dt) <= 0.0f)
			blend = 0.0f;
		
		m_animMesh->setCurrentBlend( std::fmin(blend, 1.4f) );
		//// analog animation:
		//if (vectorLen > 0)
		//	m_animMesh->setCurrentBlend(1.f);
		//else
		//	m_animMesh->setCurrentBlend(0);
	}

	// Deceleration
	if (m_state == PlayerState::FALLING)
		m_horizontalMultiplier -= PLAYER_AIR_DECELERATION * dt;
	else
		m_horizontalMultiplier -= PLAYER_DECELERATION * dt;

	if (m_horizontalMultiplier < 0.f)
		m_horizontalMultiplier = 0.f;

	// Respawning
	if (m_controller->getFootPosition().y < (float)m_heightLimitBeforeRespawn)
	{
		respawnPlayer();
	}
}

void Player::updatePlayer(const float& dt)
{
	switch (m_activeTrap)
	{
	case TrapType::EMPTY:
		break;
	case TrapType::SLOW:
		m_slowTimer += dt;
		if (m_slowTimer >= m_slowTime)
		{
			m_slowTimer = 0;
			m_currentSpeedModifier = 1;
			m_activeTrap = TrapType::EMPTY;
		}
		break;
	default:
		break;
	}


	if (m_pickupPointer)
	{
		if (m_pickupPointer->isActive())
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
				if (m_pickupPointer->isDepleted())
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

					if (m_pickupPointer->shouldDestroy())
					{
						m_pickupPointer->onRemove();
						delete m_pickupPointer;
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

Vector3 Player::getVelocity()
{
	return m_velocity;
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
	m_state = PlayerState::IDLE;
	m_controller->setPosition(m_checkpointPos);
	m_velocity = Vector3();
	m_horizontalMultiplier = 0.f;
	m_verticalMultiplier = 0.f;
}

float Player::getPlayerScale() const
{
	return this->m_playerScale;
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

const bool Player::canUsePickup()
{
	return m_pickupPointer && !m_pickupPointer->isActive();
}

void Player::handlePickupOnUse()
{
	switch (m_pickupPointer->getPickupType())
	{
	case PickupType::SPEED:
		//Not used since it is activated on use.

		//m_currentSpeedModifier = 1.f;
		//m_goalSpeedModifier = m_pickupPointer->getModifierValue();
		//m_speedModifierTime = 0;
		break;
	default:
		break;
	}
	
	m_pickupPointer->onUse();
}

void Player::inputUpdate(InputData& inputData)
{
	this->setStates(inputData.stateData);

	for (std::vector<int>::size_type i = 0; i < inputData.actionData.size(); i++)
	{
		switch (inputData.actionData[i])
		{
		case JUMP:
			if (m_state == PlayerState::IDLE || ((m_state == PlayerState::JUMPING || m_state == PlayerState::FALLING) && m_jumps < ALLOWED_NR_OF_JUMPS))
				jump();
			break;

		case DASH:
			if (m_state == PlayerState::IDLE)
			{
				if (canRoll())
					roll();
			}
			else
			{
				if (canDash())
					dash();
			}
			break;
		case USEPICKUP:
			if (canUsePickup())
				handlePickupOnUse();
			break;

		case CLOSEINTROGUI:
			GUIHandler::get().setVisible(m_instructionGuiIndex, false);
			GUIHandler::get().setVisible(closeInstructionsBtnIndex, false);
			break;

		case RESPAWN:
			respawnPlayer();
			break;

		default:
			break;
		}
	}
}

void Player::sendPhysicsMessage(PhysicsData& physicsData, bool &shouldTriggerEntityBeRemoved)
{
	if (physicsData.triggerType == TriggerType::TRAP)
	{
		switch ((TrapType)physicsData.associatedTriggerEnum)
		{
		case TrapType::SLOW:
			m_activeTrap = (TrapType)physicsData.associatedTriggerEnum;
			m_currentSpeedModifier = 0.5f;
			m_goalSpeedModifier = physicsData.floatData;
			m_speedModifierTime = 0;
			break;
		}

	}

	if (physicsData.triggerType == TriggerType::BARREL)
	{

		//spelare - barrel
		/*	Vector3 direction = ->getTranslation() - m_playerEntity->getTranslation();
		direction.Normalize();
		m_playerEntity->translate(direction);*/

		jump();
	}
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
				float duration = (float)physicsData.intData;
				switch ((PickupType)physicsData.associatedTriggerEnum)
				{
				case PickupType::SPEED:
					shouldTriggerEntityBeRemoved = true;
					m_currentSpeedModifier = 1.f;
					m_goalSpeedModifier = physicsData.floatData;
					m_speedModifierTime = 0;
					break;
				case PickupType::SCORE:
					addPickupByAssosiatedID = false;
					break;
				default:
					break;
				}
				if (addPickupByAssosiatedID)
				{
					m_pickupPointer = getCorrectPickupByID(physicsData.associatedTriggerEnum);
					m_pickupPointer->setModifierValue(physicsData.floatData);
					m_pickupPointer->onPickup(m_playerEntity, duration);
					if (m_pickupPointer->shouldActivateOnPickup())
						m_pickupPointer->onUse();
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

Pickup* getCorrectPickupByID(int id)
{
	Pickup* pickupPtr = Pickup::getPickupByID(id);
	Pickup* createPickup = nullptr;
	switch (pickupPtr->getPickupType())
	{
	case PickupType::SPEED:
		createPickup = new SpeedPickup(*dynamic_cast<SpeedPickup*>(pickupPtr));
		break;
	case PickupType::SCORE: //Score is not saved as a pointer so not implemented.
		break;
	default:
		break;
	}

	return createPickup;
}

void Player::update(GUIUpdateType type, GUIElement* guiElement)
{
	if (type == GUIUpdateType::CLICKED)
	{
		GUIHandler::get().setVisible(guiElement->m_index, false);

		//Set instructions to not visible
		GUIHandler::get().setVisible(m_instructionGuiIndex, false);
	}


	if (type == GUIUpdateType::HOVER_ENTER)
	{
		GUIButton* button = dynamic_cast<GUIButton*>(guiElement);
		button->setTexture(L"closeButtonSelected.png");
	}

	if (type == GUIUpdateType::HOVER_EXIT)
	{
		GUIButton* button = dynamic_cast<GUIButton*>(guiElement);
		button->setTexture(L"closeButton.png");
	}
}
void Player::serverPlayerAnimationChange(PlayerState currentState, float currentBlend)
{
	m_animMesh->setCurrentBlend(currentBlend);

	if (currentState != m_state)
	{
		m_state = currentState;

		switch (currentState)
		{
		case PlayerState::IDLE:
			idleAnimation();
			break;
		case PlayerState::JUMPING:

			break;
		case PlayerState::FALLING:

			break;
		case PlayerState::DASH:
			dashAnimation();
			break;
		case PlayerState::ROLL:
			rollAnimation();
			break;
		}
	}
}

void Player::jump()
{
	m_currentDistance = 0;
	m_state = PlayerState::JUMPING;
	m_verticalMultiplier = JUMP_SPEED * m_playerScale;
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

	rollAnimation();
}

bool Player::canDash() const
{
	return (m_state == PlayerState::JUMPING || m_state == PlayerState::FALLING && !m_hasDashed);
}

void Player::dash()
{
	prepDistVariables();
	m_state = PlayerState::DASH;
	m_beginDashSpeed = m_horizontalMultiplier;

	dashAnimation();
}

void Player::prepDistVariables()
{
	m_currentDistance = 0.f;
	m_moveDirection = m_playerEntity->getForwardVector();
}

void Player::rollAnimation()
{
	m_animMesh->playSingleAnimation("platformer_guy_roll1", 0.1f, false);
	m_animMesh->setAnimationSpeed(2.0f);
}

void Player::dashAnimation()
{
	m_animMesh->playSingleAnimation("platformer_guy_pose", 0.2f, true);
}

void Player::idleAnimation()
{
	m_animMesh->playBlendState("runOrIdle", 0.3f);
}
