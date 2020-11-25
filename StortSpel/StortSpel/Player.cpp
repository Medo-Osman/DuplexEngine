#include "3DPCH.h"
#include "Player.h"
#include"Pickup.h"
#include"SpeedPickup.h"
#include"HeightPickup.h"
#include"CannonPickup.h"
#include"ParticleComponent.h"
#include "Traps.h"

Pickup* getCorrectPickupByID(int id);

Player::Player()
{
	m_movementVector = XMVectorZero();
	m_jumps = 0;
	m_currentDistance = 0;
	m_hasDashed = false;
	m_transitionTime = MAX_TRANSITION_TIME;
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
		vec.emplace_back(new HeightPickup());
		vec.emplace_back(new CannonPickup());
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

	GUIImageStyle guiInfo;
	guiInfo.origin = Vector2(256, 256);
	guiInfo.position = Vector2(Engine::get().getSettings().width / 2, Engine::get().getSettings().height / 2);
	m_cannonCrosshairID = GUIHandler::get().addGUIImage(L"crosshair.png", guiInfo);
	GUIHandler::get().setVisible(m_cannonCrosshairID, false);




}

Player::~Player()
{
	if (m_playerEntity)
		delete m_playerEntity;

	//Deleted by scene
	//if (m_3dMarker)
	//	delete m_3dMarker;

	Pickup::clearStaticPickupArrayPlz();
}

void Player::setCannonEntity(Entity* entity)
{
	m_cannonEntity = entity;

	if (!m_3dMarker)
	{
		m_3dMarker = new Entity("3DMarker");
		Engine::get().getEntityMap()->emplace("3DMarker", m_3dMarker);
		MeshComponent* mesh = new MeshComponent("testCube_pCube1.lrm");
		mesh->setCastsShadow(false);
		m_3dMarker->addComponent("6 nov (mesh)", mesh);
		Engine::get().getMeshComponentMap()->emplace(1632, mesh);
	}
}

Entity* Player::get3DMarkerEntity()
{
	return m_3dMarker;
}

bool Player::isRunning()
{
	Vector3 vec = m_movementVector;
	return (vec.Length() > 0);
}

void Player::setStates(InputData& inputData)
{
	std::vector<State>& states = inputData.stateData;
	std::vector<RangeData>& range = inputData.rangeData; // Used for Gamepad left stick walking/Running
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
		for (size_t i = 0; i < range.size(); i++) // Get Analog input for walking/running
		{
			if (range[i].rangeFlag == Range::WALK)
			{
				Vector3 analogWalkW(range[i].pos.x, 0.f, range[i].pos.y);
				m_movementVector += XMVector3TransformCoord(analogWalkW, m_cameraTransform->getRotationMatrix());
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
bool doOnce = false;
Vector3 Player::calculatePath(Vector3 position, Vector3 velocity, float gravityY)
{
	bool foundEnd = false;
	bool print = !doOnce;
	float t = 0.01;
	Vector3 pos = position;
	Vector3 vel = velocity;
	while (!foundEnd && t < 200)
	{
		Vector3 curVel = vel;
		Vector3 curPos = pos;
		curVel.y -= GRAVITY * m_gravityScale * t;
		if (curVel.y <= -MAX_FALL_SPEED)
			curVel.y = -MAX_FALL_SPEED;
		curPos = curVel * t + pos;
		if (print)
		{
			std::cout << (int)curPos.x << " " << (int)curPos.y << " " << (int)curPos.z << std::endl;
			doOnce = true;
		}

		t += 0.5f;
		bool hit = Physics::get().hitSomething(curPos, m_controller->getOriginalRadius(), m_controller->getOriginalHeight());
		if (hit)
		{
			foundEnd = true;
			pos = curPos;
			m_3dMarker->setPosition(pos);
		}
		else
			m_3dMarker->setPosition(m_playerEntity->getTranslation());
	}

	ImGui::Text("Found Position:(%d, %d, %d)", (int)pos.x, (int)pos.y, (int)pos.z);

	return pos;
}

void Player::playerStateLogic(const float& dt)
{
	Vector3 directionalMovement;
	//if (m_lastDirectionalMovement.LengthSquared() == 0.f || m_lastDirectionalMovement.LengthSquared() < 0.1f)
	//{
		directionalMovement = Vector3(m_movementVector.x, 0, m_movementVector.z);
	//}
	//else
	//{
	//	//float directionDifference = XMVectorGetX(XMVector3Dot(m_lastDirectionalMovement, m_movementVector));
	//	Vector3 velocityDirection = Vector3(m_velocity.x, 0.f, m_velocity.z);
	//	velocityDirection.Normalize();
	//	directionalMovement = XMVectorLerp(velocityDirection, m_movementVector, 0.5f);
	//}

		//directionalMovement = Vector3(m_lastDirectionalMovement.x + (m_movementVector.x * directionDifference), 0, m_lastDirectionalMovement.z + (m_movementVector.z * directionDifference));
	switch (m_state)
	{
	case PlayerState::ROLL:

		if (m_currentDistance >= ROLL_TRAVEL_DISTANCE)
		{
			m_lastState = PlayerState::ROLL;

			if (m_transitionTime < MAX_TRANSITION_TIME) // Normal Movement
			{
				directionalMovement = m_moveDirection;
				// Add Speed
				m_horizontalMultiplier = PLAYER_MAX_SPEED * m_currentSpeedModifier;

				m_transitionTime += dt;
			}
			else
			{
				if (m_controller->checkGround())
				{
					m_state = PlayerState::IDLE;
					std::cout << "-------------ROLL CHANGED TO IDLE!!!\n";
				}
				else
				{
					m_state = PlayerState::FALLING;
					std::cout << "-------------ROLL CHANGED TO FALLING!!!\n";
				}
			}
			
			m_controller->setControllerSize(m_controller->getOriginalHeight());
			m_controller->setControllerRadius(m_controller->getOriginalRadius());
			idleAnimation();
		}
		else
		{
			/*Vector3 move = m_moveDirection * ROLL_SPEED * dt;
			move.y += -GRAVITY;*/
			directionalMovement = m_moveDirection;
			m_horizontalMultiplier += ROLL_SPEED * dt;

			m_currentDistance += ROLL_SPEED * dt;
			m_transitionTime = 0.f;
		}
		break;

	case PlayerState::DASH:

		if (m_currentDistance >= DASH_TRAVEL_DISTANCE)
		{
			m_lastState = PlayerState::DASH;
			m_state = PlayerState::FALLING;
			m_hasDashed = true;
			m_horizontalMultiplier = 0.f;
			m_beginDashSpeed = -1.f;
			idleAnimation();
		}
		else
		{
			/*m_currentDistance += DASH_TRAVEL_DISTANCE * DASH_SPEED;
			m_velocity += m_moveDirection * DASH_SPEED * DASH_TRAVEL_DISTANCE;*/

			float t = m_currentDistance / DASH_TRAVEL_DISTANCE;
			/*float sqt = t * t;
			float parametricBlend = sqt / (2.0f * (sqt - t) + 1.0f);*/
			//m_dashEndPosition - m_dashStartPosition;
			float speedValue = lerp(DASH_SPEED * m_currentSpeedModifier, DASH_OUT_SPEED, t); // m_beginDashSpeed is set in dash() function
			//float speedValue = lerp(m_beginDashSpeed * dt, DASH_TRAVEL_DISTANCE * m_currentSpeedModifier, parametricBlend); // m_beginDashSpeed is set in dash() function
			m_horizontalMultiplier = speedValue + 0.1f;
			m_currentDistance += (speedValue + 0.1f) * dt;
			std::cout << t << " \n";

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
		if (m_controller->checkGround())
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

		if (!m_controller->checkGround())
		{
			m_lastState = PlayerState::IDLE;
			m_state = PlayerState::FALLING;
			m_verticalMultiplier = 0.f;
		}
		break;
	case PlayerState::CANNON:
		m_controller->setPosition(m_cannonEntity->getTranslation());
		m_velocity.y = 0;

		GUIHandler::get().setVisible(m_cannonCrosshairID, false);


		if (m_shouldFire)
		{
			m_state = PlayerState::FLYINGBALL;
			m_lastState = PlayerState::CANNON;
			m_direction = m_cameraTransform->getForwardVector();
			m_velocity = m_direction;
			m_cameraOffset = Vector3(0.f, 0.f, 0.f);
			m_3dMarker->setPosition(0, -9999, -9999);
			m_shouldFire = false;
		}
		else //Draw marker
		{
			Vector3 finalPos;
			finalPos = calculatePath(m_controller->getCenterPosition(), m_cameraTransform->getForwardVector(), GRAVITY);
			
			return;
		}
		break;
	case PlayerState::FLYINGBALL:
		GUIHandler::get().setVisible(m_cannonCrosshairID, false);


		m_velocity.x = m_direction.x;
		m_velocity.z = m_direction.z;
		if (m_controller->checkGround())
		{
			PlayerMessageData data;
			data.playerActionType = PlayerActions::ON_FIRE_CANNON;
			data.playerPtr = this;

			for (int i = 0; i < m_playerObservers.size(); i++)
			{
				m_playerObservers.at(i)->reactOnPlayer(data);
			}

			m_state = PlayerState::JUMPING;
			m_lastState = PlayerState::FLYINGBALL;
			m_pickupPointer->onDepleted();
			m_pickupPointer->onRemove();
			SAFE_DELETE(m_pickupPointer);
		}
		break;
	default:
		break;
	}

	// Gravity
	if (m_state == PlayerState::FALLING || m_state == PlayerState::JUMPING || m_state == PlayerState::ROLL || m_state == PlayerState::FLYINGBALL)
	{
		m_verticalMultiplier -= GRAVITY * m_gravityScale * dt;
	}
	/*if (m_controller->checkGround(m_controller->getFootPosition(), Vector3(0.f, -1.f, 0.f), 0.1f) && m_state != PlayerState::JUMPING)
		m_verticalMultiplier = 0.f;*/

	// Max Gravity Tests
	if (m_verticalMultiplier <= -MAX_FALL_SPEED)
		m_verticalMultiplier = -MAX_FALL_SPEED;

	// Multiplier Print
	m_timeCounter += dt;
	if (m_timeCounter > 0.1f)
	{
		//std::cout << m_velocity.x << m_velocity.y << m_velocity.z << "\n";
		//std::cout << m_horizontalMultiplier << "\n";
		//std::cout << m_verticalMultiplier << "\n";
		//std::cout << m_currentSpeedModifier << "\n";

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

		m_timeCounter -= 0.1f;
	}

	// Final frame velocity
	if (directionalMovement.LengthSquared() > 0.f)
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

bool Player::pickupUpdate(Pickup* pickupPtr, const float& dt)
{
	bool shouldRemovePickup = false;

	if (pickupPtr)
	{
		if (pickupPtr->isActive())
		{
			switch (pickupPtr->getPickupType())
			{
			case PickupType::SPEED:
				m_speedModifierTime += dt;
				if (m_speedModifierTime <= FOR_FULL_EFFECT_TIME)
				{
					m_currentSpeedModifier += m_goalSpeedModifier * dt / FOR_FULL_EFFECT_TIME;
				}
				pickupPtr->update(dt);
				if (pickupPtr->isDepleted())
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

					if (pickupPtr->shouldDestroy())
					{
						shouldRemovePickup = true;
						m_currentSpeedModifier = 1.f;
					}
				}
				break;
			case PickupType::HEIGHTBOOST:
				if (m_state == PlayerState::FALLING )
				{
					pickupPtr->onDepleted();
					shouldRemovePickup = true;
				}
				break;
			case PickupType::CANNON:
				//Cannon Update
			default:
				break;
			}
		}
	}
	return shouldRemovePickup;
}

void Player::updatePlayer(const float& dt)
{
	//TRAP UPDATE
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

	//PickupUpdate
	if (pickupUpdate(m_pickupPointer, dt))
	{
		m_pickupPointer->onRemove();
		delete m_pickupPointer;
		m_pickupPointer = nullptr;
	}
	if(pickupUpdate(m_environmentPickup, dt))
	{
		m_environmentPickup->onRemove();
		delete m_environmentPickup;
		m_environmentPickup = nullptr;
	}

	//RotationUpdate
	if(m_state != PlayerState::ROLL)
		handleRotation(dt);

	//PlayerState logic Update
	playerStateLogic(dt);

	ImGui::Begin("Player Information", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
	ImGui::Text("Player Position: (%d %d, %d)", (int)this->getPlayerEntity()->getTranslation().x, (int)this->getPlayerEntity()->getTranslation().y, (int)this->getPlayerEntity()->getTranslation().z);
	ImGui::Text("PlayerState: %d", this->m_state);
	ImGui::End();
}

void Player::setPlayerEntity(Entity* entity)
{
	m_playerEntity = entity;
	m_controller = static_cast<CharacterControllerComponent*>(m_playerEntity->getComponent("CCC"));
	entity->addComponent("ScoreAudio", m_audioComponent = new AudioComponent(m_scoreSound));

	//To test heightpickup faster, if you see this you can remove it
	m_pickupPointer = new CannonPickup();
	m_pickupPointer->onPickup(m_playerEntity);
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
	if (m_pickupPointer)
	{
		if (m_pickupPointer->isActive())
		{
			m_pickupPointer->onDepleted();
			m_pickupPointer->onRemove();
			SAFE_DELETE(m_pickupPointer);

		}
	}

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

Vector3 Player::getCameraOffset()
{
	return m_cameraOffset;
}

const bool Player::canUsePickup()
{
	return m_pickupPointer && !m_pickupPointer->isActive();
}

void Player::handlePickupOnUse()
{
	PlayerMessageData data;
	data.playerActionType = PlayerActions::ON_POWERUP_USE;
	data.intEnum = (int)m_pickupPointer->getPickupType();
	data.playerPtr = this;
	switch (m_pickupPointer->getPickupType())
	{
	case PickupType::SPEED:
		//Not used since it is activated on use.

		//m_currentSpeedModifier = 1.f;
		//m_goalSpeedModifier = m_pickupPointer->getModifierValue();
		//m_speedModifierTime = 0;
		break;
	case PickupType::HEIGHTBOOST:
		//Kalla p� animation
		jump(false);
		break;
	case PickupType::CANNON:
		m_state = PlayerState::CANNON;
		m_cameraOffset = Vector3(1.f, 2.5f, 0.f);
		//Cannon on use
	default:
		break;
	}
	m_pickupPointer->onUse();
	for (int i = 0; i < m_playerObservers.size(); i++)
	{
		m_playerObservers[i]->reactOnPlayer(data);
	}
}

void Player::inputUpdate(InputData& inputData)
{

	if (m_state == PlayerState::CANNON)
	{
		for (std::vector<int>::size_type i = 0; i < inputData.actionData.size(); i++)
		{
			if (inputData.actionData.at(i) == Action::USE)
			{
				m_shouldFire = true;
			}

		}
	}


	this->setStates(inputData);

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
	
	if (!shouldTriggerEntityBeRemoved)
	{
		//Traps
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

		//Checkpoints
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

		//Pickup 
		if (physicsData.triggerType == TriggerType::PICKUP)
		{
			/*
				assosiatedTriggerEnum - PickupType
				intData - used currently for heightBoost to check if they use trampoline object or if they pickedup the item(intData = fromPickup
				stringData -free
				floatData - modifier for speedboost.
			*/
			
			if (m_pickupPointer == nullptr || ((bool)!physicsData.intData && (PickupType)physicsData.associatedTriggerEnum == PickupType::HEIGHTBOOST ))
			{
				bool environmenPickup = false;
				bool addPickupByAssosiatedID = true; // If we do not want to add pickup change this to false in switchCase.
				switch ((PickupType)physicsData.associatedTriggerEnum)
				{
				case PickupType::SPEED:
					shouldTriggerEntityBeRemoved = true; //We want to remove speedpickup entity after we've used it.
					m_currentSpeedModifier = 1.f;
					m_goalSpeedModifier = physicsData.floatData;
					m_speedModifierTime = 0;
					break;
				case PickupType::HEIGHTBOOST:
					if ((bool)physicsData.intData) //fromPickup -true/false
					{
						shouldTriggerEntityBeRemoved = true;
					}	
					else
					{
						jump(false, 1.5f);
						environmenPickup = true;
					}


					break;
				case PickupType::CANNON:
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
					Pickup* pickupPtr = getCorrectPickupByID(physicsData.associatedTriggerEnum);
					pickupPtr->setModifierValue(physicsData.floatData);


					if (environmenPickup)
					{

						if (m_environmentPickup) //If we already have an environmentpickup (SInce they can be force added, we need to, if it exist, remove the "old" enironmentPickup before creating/getting a new.
						{
							m_environmentPickup->onRemove();
							delete m_environmentPickup;
							m_environmentPickup = nullptr;
						}
						pickupPtr->onPickup(m_playerEntity);
						pickupPtr->onUse();
						m_environmentPickup = pickupPtr;
					}
					else
					{
						m_pickupPointer = pickupPtr;
						pickupPtr->onPickup(m_playerEntity);
						if (pickupPtr->shouldActivateOnPickup())
							pickupPtr->onUse();
					}
				}
			}
			//Score
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
	case PickupType::HEIGHTBOOST:
		createPickup = new HeightPickup(*dynamic_cast<HeightPickup*>(pickupPtr));
		break;
	case PickupType::CANNON:
		createPickup = new CannonPickup(*dynamic_cast<CannonPickup*>(pickupPtr));
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

void Player::jump(const bool& incrementCounter, const float& multiplier)
{
	m_currentDistance = 0;
	m_state = PlayerState::JUMPING;
	m_verticalMultiplier = JUMP_SPEED * m_playerScale;
	if(incrementCounter)
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
	m_verticalMultiplier = 0.f;
	std::cout << "ROLL PRESSED!!!!!!!!!!!!!!!!!!!!!!!!\n";
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
	m_beginDashSpeed = DASH_SPEED;

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
	m_animMesh->setAnimationSpeed(1.5f);
}

void Player::dashAnimation()
{
	m_animMesh->playSingleAnimation("platformer_guy_pose", 0.2f, true);
}

void Player::idleAnimation()
{
	m_animMesh->playBlendState("runOrIdle", 0.3f);
	m_animMesh->setAnimationSpeed(1.0f);
}
