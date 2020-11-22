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

using namespace DirectX;

enum class PlayerState
{
    IDLE,
    JUMPING,
    FALLING,
    DASH,
    ROLL,
    CANNON,
    FLYINGBALL,
};

enum class PlayerActions
{
    ON_POWERUP_USE,
    ON_FIRE_CANNON,
};


struct PlayerMessageData
{
    void* playerPtr;
    PlayerActions playerActionType;
    int intEnum;

};

class PlayerObserver {
public:
    PlayerObserver() {};
    virtual ~PlayerObserver() {};
    virtual void reactOnPlayer(PlayerMessageData& msg) = 0;
};

class PlayerSubject {
public:
    virtual ~PlayerSubject() {};
    virtual void Attach(PlayerObserver* observer) = 0;
    virtual void Detach(PlayerObserver* observer) = 0;
};



class Player : public InputObserver, public PhysicsObserver, public GUIObserver, public PlayerSubject
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
    Pickup* m_environmentPickup;
   
    //CAnnon
    Entity* m_cannonEntity;
    bool m_shouldFire = false;
    Vector3 m_direction;
    Entity* m_3dMarker;
    Vector3 m_cameraOffset;

    //
    TrapType m_activeTrap;

    Vector3 m_velocity = Vector3();
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
    int m_heightLimitBeforeRespawn = -25;

    //trap
    Vector3 m_trapPos = Vector3(0, 9, 20);
    int m_slowTime = 3;
    float m_slowTimer = 0;
    int m_trapId = -1;

    //Observer
    std::vector<PlayerObserver*> m_playerObservers;


    //Private functions
    void setStates(std::vector<State> states);
    void handleRotation(const float& dt);
    Vector3 calculatePath(Vector3 position, Vector3 velocity, float gravityY);
    void playerStateLogic(const float& dt);

    bool pickupUpdate(Pickup* pickupPtr, const float& dt);

    bool canRoll() const;
    void roll();
    bool canDash() const;
    void dash();
    void jump(const bool& incrementCounter = true, const float& multiplier = 1.0f);
    void prepDistVariables();

    void rollAnimation();
    void dashAnimation();
    void idleAnimation();
  
public:
    Player();
    ~Player();

    virtual void Attach(PlayerObserver* observer)
    {
        m_playerObservers.emplace_back(observer);
    }
    virtual void Detach(PlayerObserver* observer)
    {
        int index = -1;
        for (int i = 0; i < m_playerObservers.size() && index == -1; i++)
        {
            if (m_playerObservers[i] == observer)
                index = i;
        }
        if (index != -1)
            m_playerObservers.erase(m_playerObservers.begin() + index);
    }

    void setCannonEntity(Entity* entity);
    Entity* get3DMarkerEntity();
    Entity* getCannonEntity() { return m_cannonEntity; }
    int m_cannonCrosshairID;

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
    Vector3 getCameraOffset();
    const bool canUsePickup();
    void handlePickupOnUse();
    void inputUpdate(InputData& inputData);
    void sendPhysicsMessage(PhysicsData& physicsData, bool &removed);

    // Inherited via GUIObserver
    virtual void update(GUIUpdateType type, GUIElement* guiElement) override;
    void serverPlayerAnimationChange(PlayerState currentState, float currentBlend);
};