#pragma once
#include "Input.h"
#include "ResourceHandler.h"
#include "Entity.h"
#include"CharacterControllerComponent.h"
#include "AudioHandler.h"
#include "audioComponent.h"
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

class Player : public InputObserver
{
private:
    //CONTROLLER CONFIG
    const float CAPSULE_HEIGHT = 1.75f;
    const float CAPSULE_RADIUS = 0.5f;

    //WALK CONFIG
    const float PLAYER_SPEED = 10.f;
    DirectX::XMVECTOR m_movementVector;

    //JUMP CONFIG
    const float FALL_MULTIPLIER = 1.1f;
    //const float JUMP_DISTANCE = 30.f; //deprecated
    const float JUMP_SPEED = 15.f;
    const float JUMP_DISTANCE = 3.f;
    const int ALLOWED_NR_OF_JUMPS = 2;
    int m_jumps;

    //DASH CONFIG 
    const float DASH_TRAVEL_DISTANCE = 15.f;
    const float DASH_SPEED = 10.0f;
    bool m_hasDashed;

    //Roll CONFIG
    const float ROLL_TRAVEL_DISTANCE = 15.f;
    const float ROLL_SPEED = 10.0f;
    const float GRAVITY = 0.02f;
    const float MAX_FALL_SPEED = 9.82f;
    const float ROLL_HEIGHT = 0.3f;
    const float ROLL_RADIUS = 0.2f;


    float m_angleY;
    float m_currentDistance;
    Vector3 m_moveDirection;
    PlayerState m_state;
    Entity* m_playerEntity;
    CharacterControllerComponent* m_controller;
    Transform* m_cameraTransform;
    Vector3 m_finalMovement = Vector3();
    float m_previousVerticalMovement = 0.f;

    // Score
    int m_score;
    int m_scoreLabelGUIIndex;
    int m_scoreGUIIndex;

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

    void updatePlayer(const float& dt);
    void setPlayerEntity(Entity* entity);

    void setCameraTranformPtr(Transform* transform);
    void incrementScore();

    int getScore();
    Entity* getPlayerEntity() const;
    void inputUpdate(InputData& inputData);
};