#pragma once
#include "Input.h"
#include "ResourceHandler.h"
#include "Entity.h"
#include "AudioHandler.h"
#include "audioComponent.h"

#include <cmath>

using namespace DirectX;

class Player : public InputObserver
{
private:
    DirectX::XMVECTOR m_movementVector;
    Entity* m_playerEntity = nullptr;
    Transform* m_cameraTransform;
    const int m_playerSpeed = 10;
    float angleY;

    void setStates(std::vector<State> states);

public:
    Player();

    void updatePlayer(const float& dt);
    void setPlayerEntity(Entity* entity);

    void setCameraTranformPtr(Transform* transform);

    Entity* getPlayerEntity();
    void inputUpdate(InputData& inputData);
};