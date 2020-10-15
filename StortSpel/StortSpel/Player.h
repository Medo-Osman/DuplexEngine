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
    const float FALL_MULTIPLIER = 2.5f;
    const float MAX_FALL = 0.005f;
    const float GRAVITY_MODIFIER = -0.07f;
    const float INITAL_JUMP_VELOCITY = 0.05f;
    DirectX::XMVECTOR m_movementVector;
    Entity* m_playerEntity = nullptr;
    Transform* m_cameraTransform;
    const float m_playerSpeed = 0.007f;
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