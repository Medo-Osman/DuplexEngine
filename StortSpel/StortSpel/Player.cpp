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

	for (int i = 0; i < 10; i++)
	{
		m_lineData[i].direction = { 0, 0, 0 };
		m_lineData[i].position = { 0, 0, 0 };
	}
	
	//GUI
	m_score = 0;
	//style.position.y = 70.f;
	//style.scale = { 0.5f };
	//m_scoreLabelGUIIndex = GUIHandler::get().addGUIText("Score: ", L"squirk.spritefont", style);
	GUIImageStyle iStyle;
	iStyle.position = Vector2(1750, 150);
	m_scoreBG_GUIIndex = GUIHandler::get().addGUIImage(L"Power-up_BG.png", iStyle);
	iStyle.position = Vector2(1700, 50);
	m_scoreBG_GUIIndex = GUIHandler::get().addGUIImage(L"Point_BG.png", iStyle);
	GUITextStyle style;
	style.position = Vector2(1717, 62);
	style.color = Colors::White;
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

void Player::setCannonEntity(Entity* entity, MeshComponent* pipe)
{
	m_cannonEntity = entity;
	m_pipe = pipe;
	m_shouldDrawLine = true;

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
	if (m_state != PlayerState::DASH && m_state != PlayerState::ROLL && m_state != PlayerState::CANNON && m_state != PlayerState::FLYINGBALL)
	{
		bool hasJumped = false;
		Quaternion cameraRot = m_cameraTransform->getRotation();
		for (size_t i = 0; i < states.size(); i++)
		{
			switch (states[i])
			{
			case WALK_LEFT:		m_movementVector += XMVector3Rotate(Vector3(-1.f, 0.f, 0.f), Vector4(0.f, cameraRot.y, 0.f, cameraRot.w)); break;
			case WALK_RIGHT:	m_movementVector += XMVector3Rotate(Vector3(1.f, 0.f, 0.f), Vector4(0.f, cameraRot.y, 0.f, cameraRot.w)); break;
			case WALK_FORWARD:	m_movementVector += XMVector3Rotate(Vector3(0.f, 0.f, 1.f), Vector4(0.f, cameraRot.y, 0.f, cameraRot.w)); break;
			case WALK_BACKWARD: m_movementVector += XMVector3Rotate(Vector3(0.f, 0.f, -1.f), Vector4(0.f, cameraRot.y, 0.f, cameraRot.w)); break;
			case JUMPING:
				hasJumped = true;
				m_lastJumpPressed = m_jumpPressed;
				m_jumpPressed = true;
				break;
			default: break;
			}
		}
		if (!hasJumped)
		{
			m_lastJumpPressed = m_jumpPressed;
			m_jumpPressed = false;
		}

		for (size_t i = 0; i < range.size(); i++) // Get Analog input for walking/running
		{
			if (range[i].rangeFlag == Range::WALK)
			{
				Vector3 analogWalkW(range[i].pos.x, 0.f, range[i].pos.y);
				Quaternion cameraRot = m_cameraTransform->getRotation();
				m_movementVector += XMVector3Rotate(analogWalkW, Vector4(0.f , cameraRot.y, 0.f, cameraRot.w));
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

	if (Vector3(m_movementVector).LengthSquared() > 0.f)
	{
		//This is the current rotation in quaternions
		currentRotation = m_playerEntity->getRotation();
		currentRotation.Normalize();

		auto cameraRot = m_cameraTransform->getRotation();
		
		Quaternion cameraRotationY = Quaternion(0.f, cameraRot.y, 0.f, cameraRot.w);
		cameraRotationY.Normalize();
		Vector3 cameraForward = XMVector3Rotate(Vector4(0.f, 0.f, 1.f, 0.f), cameraRotationY);
		
		auto offset = Vector4(XMVector3AngleBetweenNormals(XMVector3Normalize(m_movementVector), cameraForward));

		//if this vector has posisitv value the character is facing the positiv x axis, checks movementVec against cameraForward
		if (XMVectorGetY(XMVector3Cross(m_movementVector, cameraForward)) > 0.0f)
			offset.y = -offset.y;

		//This is the angleY target quaternion
		targetRot = Quaternion(0, cameraRot.y, 0, cameraRot.w) * Quaternion::CreateFromYawPitchRoll(offset.y, 0, 0);
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

Vector3 Player::trajectoryEquation(Vector3 pos, Vector3 dir, float t, float horizonalMultiplier, float vertMulti)
{
	Vector3 curPos = pos;
	Vector3 curDir = dir;

	return curPos;
}

void Player::trajectoryEquationOutFill(Vector3 pos, Vector3 dir, float t, float horizonalMultiplier, float vertMulti, XMFLOAT3& outPos, XMFLOAT3& outDir)
{

}
bool hasPrinted = false;
																	//m_horizontalMultiplier, m_verticalMultiplier
Vector3 Player::calculatePath(Vector3 position, Vector3 direction, float horizonalMultiplier, float vertMulti)
{
	Vector3 returnPosition;

	bool foundEnd = false;
	float t = 5.0f;
	Vector3 pos = position;
	Vector3 dir = direction;

	while (!foundEnd && t < 5)
	{
		pos = trajectoryEquation(position, dir, t, horizonalMultiplier, vertMulti);

		t += 0.01f;
		bool hit = Physics::get().hitSomething(pos, m_controller->getOriginalRadius(), m_controller->getOriginalHeight());

		if (hit)
		{
			m_shouldDrawLine = true;
			foundEnd = true;
			returnPosition = pos;
			m_3dMarker->setPosition(returnPosition);
			float tDist = t / 10;
			for (int i = 0; i < 10; i++)
			{
				Vector3 lineDataPos;
				Vector3 lineDataDir;
				float tempT = tDist * i;
				trajectoryEquationOutFill(position, dir, tempT, horizonalMultiplier, vertMulti, m_lineData[i].position, this->m_lineData[i].direction);
			}

		}
		else
		{
			m_3dMarker->setPosition(m_playerEntity->getTranslation());
		}
	}

	if (!foundEnd)
	{
		m_3dMarker->setPosition(pos);
		float tDist = t / 10;
		for (int i = 0; i < 10; i++)
		{
			Vector3 lineDataPos;
			Vector3 lineDataDir;
			float tempT = tDist * i;
			trajectoryEquationOutFill(position, dir, tempT, horizonalMultiplier, vertMulti, m_lineData[i].position, this->m_lineData[i].direction);
		}
	}

	hasPrinted = true;
	return returnPosition;
}

void Player::playerStateLogic(const float& dt)
{
	Vector3 directionalMovement = Vector3(m_movementVector.x, 0, m_movementVector.z);

	float currentY = m_playerEntity->getTranslation().y;

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
					m_state = PlayerState::IDLE;
				else
					m_state = PlayerState::FALLING;
			}
			
			m_controller->setControllerSize(m_controller->getOriginalHeight());
			m_cameraOffset = ORIGINAL_CAMERA_OFFSET;
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
			float speedValue = lerp(DASH_SPEED * m_currentSpeedModifier, DASH_OUT_SPEED, t);
			m_horizontalMultiplier = speedValue + 0.1f;
			m_currentDistance += (speedValue + 0.1f) * dt;

			directionalMovement = m_moveDirection;
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
			if (m_jumps == 1)
				endJump_First();
			else
				endJump_Second();
			m_lastState = PlayerState::FALLING;
			m_state = PlayerState::IDLE;
			m_jumps = 0;
			m_hasDashed = false;
		}
		else if (m_jumpPressed && !m_lastJumpPressed && m_jumps < ALLOWED_NR_OF_JUMPS)
			jump(dt);
		
		break;

	case PlayerState::JUMPING:


		if (m_jumps == 1)
			m_jumpLimit = JUMP_HEIGHT_FORCE_LIMIT;
		else
			m_jumpLimit = JUMP_HEIGHT_FORCE_LIMIT * .8f;

		if (m_shouldPickupJump)
			m_jumpLimit = JUMP_HEIGHT_FORCE_LIMIT * TRAMPOLINE_JUMP_MULTIPLIER;

		if ((m_jumpPressed || m_shouldPickupJump) && (currentY - m_jumpStartY < m_jumpLimit)) // Jump Limit Not Reached
		{
			m_verticalMultiplier += JUMP_SPEED * dt; // Apply Jump Force
		}

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
		else if (m_jumpPressed && !m_lastJumpPressed && m_jumps < ALLOWED_NR_OF_JUMPS)
			jump();

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
		else if (m_jumpPressed && !m_lastJumpPressed)
			jump();

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
			m_cameraOffset = ORIGINAL_CAMERA_OFFSET;
			m_3dMarker->setPosition(0, -9999, -9999);
			m_shouldFire = false;
			m_shouldDrawLine = false;
			m_verticalMultiplier = 1;
			m_horizontalMultiplier = CANNON_POWER;

			directionalMovement = m_cameraTransform->getForwardVector();

			PlayerMessageData data;
			data.playerActionType = PlayerActions::ON_FIRE_CANNON;
			data.playerPtr = this;

			static_cast<CannonPickup*>(m_pickupPointer)->geFyr();

			for (int i = 0; i < m_playerObservers.size(); i++)
			{
				m_playerObservers.at(i)->reactOnPlayer(data);
			}
		}
		else //Draw marker
		{
			Vector3 finalPos;
			finalPos = calculatePath(m_controller->getCenterPosition(), m_cameraTransform->getForwardVector() * 10, CANNON_POWER, 1);
			
			/*            Set base rot (Y)              */
			Quaternion q1 = m_cameraTransform->getRotation();
			Quaternion q2(0, q1.y, 0, q1.w);
			q2.Normalize();
			Matrix matrix = XMMatrixRotationQuaternion(q2);
			m_cannonEntity->setRotation(matrix);

			/*           Set pipe rot (X)               */
			q1 = m_cameraTransform->getRotation();
			q2 = Quaternion(q1.x, 0, 0, q1.w);
			q2.Normalize();
			matrix = XMMatrixRotationQuaternion(q2);
			m_pipe->setRotation(matrix);

			return;
		}
		break;
	case PlayerState::FLYINGBALL:
		GUIHandler::get().setVisible(m_cannonCrosshairID, false);
		
		if (m_controller->castRay(m_controller->getCenterPosition(), DirectX::XMVector3Normalize(m_direction), 1.f) || m_controller->checkGround() || m_horizontalMultiplier < 0.1f)
		{
			m_horizontalMultiplier = 0;
			m_verticalMultiplier = 0;
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


		/*switch (m_state)
		{
		case PlayerState::DASH:		std::cout << "DASH\n"; break;
		case PlayerState::ROLL:		std::cout << "ROLL\n"; break;
		case PlayerState::JUMPING:	std::cout << "JUMPING\n"; break;
		case PlayerState::FALLING:	std::cout << "FALLING\n"; break;
		case PlayerState::IDLE:		std::cout << "IDLE\n"; break;
		default: break;
		}
		std::cout << "\n";*/

		m_timeCounter -= 0.1f;
	}

	
	// Final frame velocity
	if (directionalMovement.LengthSquared() > 0.f) 
	{
		// Compensate for larger directional change
		m_horizontalMultiplier = m_horizontalMultiplier * max(directionalMovement.Dot(m_lastDirectionalMovement), 0.2f);
		m_lastDirectionalMovement = directionalMovement;
	}

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

		m_animMesh->setCurrentBlend( std::fmin(blend, 1.55f) );

		//// analog animation:
		//if (vectorLen > 0)
		//	m_animMesh->setCurrentBlend(1.f);
		//else
		//	m_animMesh->setCurrentBlend(0);
	}

	// Deceleration
	if (m_state == PlayerState::FALLING)
		m_horizontalMultiplier -= PLAYER_AIR_DECELERATION * m_currentSpeedModifier * dt;
	else
		m_horizontalMultiplier -= PLAYER_DECELERATION * m_currentSpeedModifier * dt;

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
			pickupPtr->update(dt);
			switch (pickupPtr->getPickupType())
			{
			case PickupType::SPEED:
				m_speedModifierTime += dt;
				if (m_speedModifierTime <= FOR_FULL_EFFECT_TIME)
				{
					m_currentSpeedModifier += m_goalSpeedModifier * dt / FOR_FULL_EFFECT_TIME;
				}

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
					m_shouldPickupJump = false;
					shouldRemovePickup = true;
				}
				break;
			case PickupType::CANNON:
				//Cannon Update
				break;
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
	if (m_respawnNextFrame)
		respawnPlayer();

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
	m_pickupPointer = new HeightPickup();
	m_pickupPointer->onPickup(m_playerEntity, false);
}

Vector3 Player::getCheckpointPos()
{
	return m_checkpointPos;
}

Vector3 Player::getVelocity()
{
	return m_velocity;
}


LineData* Player::getLineDataArray()
{
	return m_lineData;
}	
PlayerState Player::getState()
{
	return m_state;
}

PlayerState Player::getLastState()
{
	return m_lastState;
}

Vector3 Player::getFeetPosition()
{
	return m_controller->getFootPosition();
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
	
	if (m_score >= 10 && m_score < 100)
	{
		GUITextStyle style;
		style.position = Vector2(1705, 62);
		style.color = Colors::White;
		GUIHandler::get().setGUITextStyle(m_scoreGUIIndex, style);
	}
	if (m_score >= 100)
	{
		
		GUITextStyle style;
		style.position = Vector2(1678, 62);
		style.color = Colors::White;
		GUIHandler::get().setGUITextStyle(m_scoreGUIIndex, style);
	}
}

void Player::respawnPlayer()
{
	m_respawnNextFrame = false;
	m_state = PlayerState::IDLE;
	m_controller->setPosition(m_checkpointPos);
	m_velocity = Vector3();
	m_horizontalMultiplier = 0.f;
	m_verticalMultiplier = 0.f;
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
		//jump(false, TRAMPOLINE_JUMP_MULTIPLIER); //Activates by enviormental instead.
		//m_shouldPickupJump = true;
		break;
	case PickupType::CANNON:
		m_state = PlayerState::CANNON;
		m_cameraOffset = Vector3(1.5f, 4.0f, 0.f);
		//Cannon on use
		break;
	default:
		break;
	}
	m_pickupPointer->onUse();
	for (int i = 0; i < m_playerObservers.size(); i++)
	{
		m_playerObservers[i]->reactOnPlayer(data);
	}
}

void Player::sendPlayerMSG(const PlayerMessageData &data)
{
	for (int i = 0; i < (int)m_playerObservers.size(); i++)
	{
		m_playerObservers.at(i)->reactOnPlayer(data);
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

			if (m_pickupPointer == nullptr || ((bool)!physicsData.intData && (PickupType)physicsData.associatedTriggerEnum == PickupType::HEIGHTBOOST))
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
						jump(false, TRAMPOLINE_JUMP_MULTIPLIER);
						m_shouldPickupJump = true;
						environmenPickup = true;
						PlayerMessageData data;
						data.playerActionType = PlayerActions::ON_ENVIRONMENTAL_USE;
						data.intEnum = physicsData.associatedTriggerEnum;
						data.entityIdentifier = physicsData.entityIdentifier;

						for (int i = 0; i < (int)m_playerObservers.size(); i++)
						{
							m_playerObservers.at(i)->reactOnPlayer(data);
						}
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
						pickupPtr->onPickup(m_playerEntity, true);
						pickupPtr->onUse();
						m_environmentPickup = pickupPtr;
					}
					else
					{
						m_pickupPointer = pickupPtr;
						pickupPtr->onPickup(m_playerEntity, false);
						if (pickupPtr->shouldActivateOnPickup())
							pickupPtr->onUse();
					}
				}
			}
			//Score
			if ((PickupType)physicsData.associatedTriggerEnum == PickupType::SCORE)
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
	if (incrementCounter)
	{
		m_state = PlayerState::JUMPING;
		m_jumps++;
	}
	else
		m_state = PlayerState::JUMPING;

	m_jumpStartY = m_playerEntity->getTranslation().y;

	if (m_jumps == 1)
		startJump_First();
	else
		startJump_Second();

	m_currentDistance = 0;
	m_verticalMultiplier = JUMP_START_SPEED * multiplier; // Apply Jump Force
}

bool Player::canRoll() const
{
	return (m_state == PlayerState::IDLE);
}

void Player::roll()
{
	prepDistVariables();
	m_controller->setControllerSize(ROLL_HEIGHT);
	m_state = PlayerState::ROLL;
	m_verticalMultiplier = 0.f;
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

	m_animMesh->playSingleAnimation("Slide", 0.2f, false, false);
	m_animMesh->setAnimationSpeed(1.2f);
}

void Player::dashAnimation()
{
	m_animMesh->playSingleAnimation("Dash", 0.1f, false, true);
	m_animMesh->setAnimationSpeed(1.f);
}

void Player::idleAnimation()
{
	m_animMesh->playBlendState("runOrIdle", 0.3f);
	m_animMesh->setAnimationSpeed(1.5f);
}

void Player::startJump_First()
{
	m_animMesh->playSingleAnimation("JumpStart_First", 0.01f, true, false);
	m_animMesh->queueSingleAnimation("JumpLoop_First", 0.f, true, false);
}

void Player::endJump_First()
{
	m_animMesh->playSingleAnimation("JumpEnd_First", 0.01f, false, false);
	//m_animMesh->setAnimationSpeed(0.1f);
	m_animMesh->queueBlendState("runOrIdle", 0.05f);
}

void Player::startJump_Second()
{
	m_animMesh->playSingleAnimation("JumpStart_Second", 0.1f, true, false);
	m_animMesh->queueSingleAnimation("JumpLoop_Second", 0.1f, true, false);
}

void Player::endJump_Second()
{
	m_animMesh->playSingleAnimation("JumpEnd_Second", 0.1f, false, false);
	m_animMesh->queueBlendState("runOrIdle", 0.3f);
}
