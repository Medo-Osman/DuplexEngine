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
    ON_ENVIRONMENTAL_USE,
    ON_FIRE_CANNON,
};


struct PlayerMessageData
{
    void* playerPtr;
    PlayerActions playerActionType;
    int intEnum;
    std::string entityIdentifier;

};

class PlayerObserver {
public:
    PlayerObserver() {};
    virtual ~PlayerObserver() {};
    virtual void reactOnPlayer(const PlayerMessageData& msg) = 0;
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
    //WALK CONFIG
    const float PLAYER_MAX_SPEED = 8.f;
    const float PLAYER_ACCELERATION = 30.f; // times dt
    const float PLAYER_DECELERATION = 15.f; // times dt
    const float PLAYER_ROTATION_SPEED = 0.08f;
    float m_verticalMultiplier = 0.f;
    float m_horizontalMultiplier = 0.f;
    Vector3 m_movementVector;

    //JUMP CONFIG
    const float JUMP_SPEED = 70.f;
    const float JUMP_START_SPEED = 4.f;
    //const float JUMP_SPEED = 10.f;
    const float PLAYER_AIR_ACCELERATION = 20.f;
    const float PLAYER_AIR_DECELERATION = 15.f;
    const float JUMP_HEIGHT_FORCE_LIMIT = 1.0f;
    float m_jumpLimit = JUMP_HEIGHT_FORCE_LIMIT;

    bool m_jumpPressed = false;
    bool m_lastJumpPressed = false;
    float m_jumpStartY = 0.f;
    const int ALLOWED_NR_OF_JUMPS = 2;
    int m_jumps;

    //FALLING / GRAVITY CONFIG
    const float GRAVITY = 9.82f; // times dt
    const float MAX_FALL_SPEED = 20.f;
    float m_gravityScale = 4.f;

    //DASH CONFIG
    const float DASH_TRAVEL_DISTANCE = 3.f;
    const float DASH_SPEED = 30.0f;
    const float DASH_OUT_SPEED = 10.0f;
    float m_beginDashSpeed = -1.f;
    bool m_hasDashed;

    //Roll CONFIG
    const float ROLL_TRAVEL_DISTANCE = 30.f;
    const float ROLL_SPEED = 50.0f;
    const float ROLL_HEIGHT = 0.2f;
    const float ROLL_RADIUS = 0.2f; // not used
    const float ROLL_TRANSITION_SPEED = 8.0f;
    const float MAX_TRANSITION_TIME = 0.2f; // Sec
    float m_transitionTime;

    //Cannon Config
    const float CANNON_POWER = 100;


    // Speed Powerup
    float m_currentSpeedModifier;
    float m_goalSpeedModifier;
    int m_speedModifierDuration;
    float m_speedModifierTime;
    const float FOR_FULL_EFFECT_TIME = 2.f;

    // Pickup
    Pickup* m_pickupPointer;
    Pickup* m_environmentPickup;

    // UI
    int m_instructionGuiIndex = 0;
    int closeInstructionsBtnIndex = 0;

    // Movement
    float m_currentDistance;
    Vector3 m_moveDirection;
    Vector3 m_velocity = Vector3();
    Vector3 m_lastDirectionalMovement = Vector3();
    Vector3 m_lastPosition = Vector3();
    float m_previousVerticalMovement = 0.f;

    // States
    PlayerState m_state;
    PlayerState m_lastState;

    // Entity and Components
    Entity* m_playerEntity;
    AnimatedMeshComponent* m_animMesh;
    CharacterControllerComponent* m_controller;

    // Camera
    Transform* m_cameraTransform;
    const Vector3 ORIGINAL_CAMERA_OFFSET = Vector3(0.f, 0.0f, 0.0f);
    Vector3 m_cameraOffset = ORIGINAL_CAMERA_OFFSET;

    //Cannon
    Entity* m_cannonEntity;
    bool m_shouldFire = false;
    Vector3 m_direction;
    Entity* m_3dMarker;
    LineData m_lineData[10];
    MeshComponent* m_pipe;
    float y;

    //Trampoline
    bool m_shouldPickupJump;
    const float TRAMPOLINE_JUMP_MULTIPLIER = 2.0f;
    std::string m_trampolineEntityIdentifier; //Used to make sure player does not use same trampoline object twice, so we don't recive double sounds for example.

    // Trap
    TrapType m_activeTrap;

    // Score
    int m_score;
    int m_scoreLabelGUIIndex;
    int m_scoreGUIIndex;
    int m_scoreBG_GUIIndex;
    int m_powerUp_GUIIndex;
    std::wstring m_scoreSound = L"StarSound.wav";
    AudioComponent* m_audioComponent;

    //Checkpoint
    Vector3 m_checkpointPos = Vector3(0.f, 9.f, 5.f);
    int m_heightLimitBeforeRespawn = -10.f;

    //trap
    Vector3 m_trapPos = Vector3(0, 9, 20);
    int m_slowTime = 3;
    float m_slowTimer = 0;
    int m_trapId = -1;

    //Observer
    std::vector<PlayerObserver*> m_playerObservers;


    //Private functions
    void setStates(InputData& inputData);
    void handleRotation(const float& dt);
    Vector3 trajectoryEquation(Vector3 position, Vector3 direction, float t, float horizonalMultiplier, float vertMulti);
	void trajectoryEquationOutFill(Vector3 position, Vector3 direction, float t, float horizonalMultiplier, float vertMulti, XMFLOAT3& outPos, XMFLOAT3& outDir);
    Vector3 calculatePath(Vector3 position, Vector3 direction, float horizonalMultiplier, float vertMulti);
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
    void startJump_First();
    void endJump_First();
    void startJump_Second();
    void endJump_Second();

    bool m_respawnNextFrame = false; //PhysX won't allow you to read and write at the same time.

public:
    Player();
    ~Player();
    bool m_shouldDrawLine = false;


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

    void setCannonEntity(Entity* entity, MeshComponent* pipe);
    Entity* get3DMarkerEntity();
    Entity* getCannonEntity() { return m_cannonEntity; }
    int m_cannonCrosshairID;

    bool isRunning();

    void updatePlayer(const float& dt);

    void setPlayerEntity(Entity* entity);

    Vector3 getCheckpointPos();
    Vector3 getVelocity();
    LineData* getLineDataArray();
    PlayerState getState();
    PlayerState getLastState();
    Vector3 getFeetPosition();
    void setCheckpoint(Vector3 newPosition);

    void setCameraTranformPtr(Transform* transform);
    void setAnimMeshPtr(AnimatedMeshComponent* animatedMesh);

    void incrementScore();

    void increaseScoreBy(int value);
    void respawnPlayer();


    int m_nrOfBarrelDrops = 0;
    int getScore();
    void setScore(int newScore);
    Entity* getPlayerEntity() const;
    Vector3 getCameraOffset();
    const bool canUsePickup();
    void handlePickupOnUse();
    void sendPlayerMSG(const PlayerMessageData& data);
    void inputUpdate(InputData& inputData);
    void sendPhysicsMessage(PhysicsData& physicsData, bool &removed);

    // Inherited via GUIObserver
    virtual void update(GUIUpdateType type, GUIElement* guiElement) override;
    void serverPlayerAnimationChange(PlayerState currentState, float currentBlend);
};
