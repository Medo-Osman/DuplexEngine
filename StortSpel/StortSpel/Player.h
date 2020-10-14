#pragma once
#include "Input.h"
#include "ResourceHandler.h"
#include "Entity.h"
#include"CharacterControllerComponent.h"
#include "AudioHandler.h"
#include "audioComponent.h"
#include <cmath>

enum class PlayerState
{
    IDLE,
    DO_JUMP,
    JUMPING,
};

using namespace DirectX;

class Player : public InputObserver
{
private:
    const float MAX_FALL_SPEED = -0.09f;
    const float GRAVITY_MODIFIER = -0.2f;
    const float INITAL_JUMP_VELOCITY = 0.15f;
    DirectX::XMVECTOR m_movementVector;
    Entity* m_playerEntity = nullptr;
    Transform* m_cameraTransform;
    const int m_playerSpeed = 10;
    float angleY;
    float m_height;
    int m_jumps;
    PlayerState m_state;

    void setStates(std::vector<State> states);
    CharacterControllerComponent* m_controller;

public:
    Player();

    void updatePlayer(const float& dt);
    void setPlayerEntity(Entity* entity);

    void setCameraTranformPtr(Transform* transform);

    Entity* getPlayerEntity();
    void inputUpdate(InputData& inputData);
};