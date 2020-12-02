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
#include "Traps.h"
#include "SlowTrapComponent.h"
#include "PushTrapComponent.h"
#include "GUIHandler.h"
#include "BarrelComponent.h"

enum class PlayerState
{
    IDLE,
    JUMPING,
    FALLING,
    DASH,
    ROLL,
};

using namespace DirectX;

class Player : public InputObserver, public PhysicsObserver, public GUIObserver
{
private:
    float m_playerScale = 2.0f;

    //CONTROLLER CONFIG
    const float CAPSULE_HEIGHT = 1.5f;
    const float CAPSULE_RADIUS = 0.01f;

    //WALK CONFIG
    const float PLAYER_SPEED = 10.f;
    DirectX::XMVECTOR m_movementVector;

    //JUMP CONFIG
    const float FALL_MULTIPLIER = 1.1f;
    //const float JUMP_DISTANCE = 30.f; //deprecated
    const float JUMP_SPEED = 0.10f;
    //const float JUMP_DISTANCE = 3.f;
    const int ALLOWED_NR_OF_JUMPS = 2;
    int m_jumps;

    float m_gravityScale = .0005f;

    //DASH CONFIG 
    const float DASH_TRAVEL_DISTANCE = 7.f;
    const float DASH_SPEED = 10.0f;
    bool m_hasDashed;

    //Roll CONFIG
    const float ROLL_TRAVEL_DISTANCE = 10.f;
    const float ROLL_SPEED = 15.0f;
    PlayerState m_lastState;
    const float GRAVITY = 9.82f;
    //const float GRAVITY = 0.375f;
    const float MAX_FALL_SPEED = 0.3f;
    const float ROLL_HEIGHT = 0.3f;
    const float ROLL_RADIUS = 0.2f;

    float m_currentSpeedModifier;
    float m_goalSpeedModifier;
    int m_speedModifierDuration;
    float m_speedModifierTime;
    const float FOR_FULL_EFFECT_TIME = 2.f;

    int m_instructionGuiIndex = 0;

    int closeInstructionsBtnIndex = 0;

    float m_angleY;
    float m_currentDistance;
    Vector3 m_moveDirection;
    PlayerState m_state;
    Entity* m_playerEntity;
    AnimatedMeshComponent* m_animMesh;
    CharacterControllerComponent* m_controller;
    Transform* m_cameraTransform;
    Pickup* m_pickupPointer;
   
    //
    TrapType m_activeTrap;

    Vector3 m_velocity = Vector3();
    Vector3 m_lastPosition = Vector3();
    float m_previousVerticalMovement = 0.f;

    // Score
    int m_score;
    int m_scoreLabelGUIIndex;
    int m_scoreGUIIndex;
    int m_scoreBG_GUIIndex;
    int m_powerUp_GUIIndex;
    std::wstring m_scoreSound = L"StarSound.wav";
    AudioComponent* m_audioComponent;

    //Checkpoint
    Vector3 m_checkpointPos = Vector3(0, 9, 5);
    int m_heightLimitBeforeRespawn = -25;

    //trap
    Vector3 m_trapPos = Vector3(0, 9, 20);
    int m_slowTime = 3;
    float m_slowTimer = 0;
    int m_trapId = -1;

    void setStates(std::vector<State> states);
    void handleRotation(const float& dt);
    void playerStateLogic(const float& dt);

    bool canRoll() const;
    void roll();
    bool canDash() const;
    void dash();
    void jump();
    void prepDistVariables();

    void rollAnimation();
    void dashAnimation();
    void idleAnimation();
  
public:
    Player();
    ~Player();

    bool isRunning();

    void updatePlayer(const float& dt);
    
    void setPlayerEntity(Entity* entity);

    Vector3 getCheckpointPos();
    Vector3 getVelocity();
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
    const bool canUsePickup();
    void handlePickupOnUse();
    void inputUpdate(InputData& inputData);
    void sendPhysicsMessage(PhysicsData& physicsData, bool &removed);

    // Inherited via GUIObserver
    virtual void update(GUIUpdateType type, GUIElement* guiElement) override;
    void serverPlayerAnimationChange(PlayerState currentState, float currentBlend);
};