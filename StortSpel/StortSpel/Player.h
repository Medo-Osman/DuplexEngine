#pragma once
#include "Input.h"
#include "ResourceHandler.h"
#include "Entity.h"
#include "CharacterControllerComponent.h"
#include "AnimatedMeshComponent.h"
#include "AudioHandler.h"
#include "audioComponent.h"
#include <cmath>
#include"Physics.h"
#include"Pickup.h"
#include"CheckpointComponent.h"

#include "GUIHandler.h"

enum class PlayerState
{
    IDLE,
    JUMPING,
    FALLING,
    DASH,
    ROLL,
};

using namespace DirectX;
using namespace SimpleMath;

class Player : public InputObserver, public PhysicsObserver
{
private:
    float m_playerScale = 2.0f;

    //CONTROLLER CONFIG
    const float CAPSULE_HEIGHT = 1.5f;
    const float CAPSULE_RADIUS = 0.01f;

    //WALK CONFIG
    const float PLAYER_MAX_SPEED = 10.f;
    const float PLAYER_ACCELERATION = 20.f;
    const float PLAYER_DECELERATION = 10.f;
    float m_verticalMultiplier;
    float m_horizontalMultiplier;
    Vector3 m_movementVector;
    float m_timeCounter = 0.f;

    //JUMP CONFIG
    const float JUMP_SPEED = 10.f;
    const int ALLOWED_NR_OF_JUMPS = 2;
    int m_jumps;

    // Gravity
    const float GRAVITY = 9.82f;
    const float MAX_FALL_SPEED = 20.f;
    float m_gravityScale = 3.f;
    PlayerState m_lastState;

    //DASH CONFIG 
    const float DASH_TRAVEL_DISTANCE = 7.f;
    const float DASH_SPEED = 10.0f;
    bool m_hasDashed;

    //Roll CONFIG
    const float ROLL_TRAVEL_DISTANCE = 10.f;
    const float ROLL_SPEED = 15.0f;
    const float ROLL_HEIGHT = 0.3f;
    const float ROLL_RADIUS = 0.2f;

    float m_currentSpeedModifier;
    float m_goalSpeedModifier;
    int m_speedModifierDuration;
    float m_speedModifierTime;
    const float FOR_FULL_EFFECT_TIME = 2.f;

    int m_instructionGuiIndex = 0;


    float m_angleY;
    float m_currentDistance;
    Vector3 m_moveDirection;
    PlayerState m_state;
    Entity* m_playerEntity;
    AnimatedMeshComponent* m_animMesh;
    CharacterControllerComponent* m_controller;
    Transform* m_cameraTransform;
    Pickup* m_pickupPointer;

    Vector3 m_velocity = Vector3();
    Vector3 m_lastDirectionalMovement;
    Vector3 m_lastPosition = Vector3();
    float m_previousVerticalMovement = 0.f;

    // Score
    int m_score;
    int m_scoreLabelGUIIndex;
    int m_scoreGUIIndex;
    std::wstring m_scoreSound = L"StarSound.wav";
    AudioComponent* m_audioComponent;

    //Checkpoint
    Vector3 m_checkpointPos = Vector3(0, 9, 5);
    int m_heightLimitBeforeRespawn = -15;

    void setStates(std::vector<State> states);
    void handleRotation(const float& dt);
    void playerStateLogic(const float& dt);

    bool canRoll() const;
    void roll();
    bool canDash() const;
    void dash();
    void jump();
    void prepDistVariables();

    
  
public:
    Player();
    ~Player();

    void updatePlayer(const float& dt);
    
    void setPlayerEntity(Entity* entity);

    Vector3 getCheckpointPos();
    void setCheckpoint(Vector3 newPosition);

    void setCameraTranformPtr(Transform* transform);
    void setAnimMeshPtr(AnimatedMeshComponent* animatedMesh);
    
    void incrementScore();

    void increaseScoreBy(int value);
    void respawnPlayer();

    float getPlayerScale() const;

    int getScore();
    void setScore(int newScore);
    Entity* getPlayerEntity() const;
    void inputUpdate(InputData& inputData);
    void sendPhysicsMessage(PhysicsData& physicsData, bool &removed);
};