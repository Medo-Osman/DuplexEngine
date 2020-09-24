#pragma once
#include "Input.h"
#include "ResourceHandler.h"
#include "Entity.h"

using namespace DirectX;

class Player : public InputObserver
{
private:
    DirectX::XMVECTOR m_movementVector;
    Entity* m_playerEntity = nullptr;
    void setStates(std::vector<State> states);
public:
    Player();

    void updatePlayer(const float& dt);
    void setPlayerEntity(Entity* entity);
    Entity* getPlayerEntity();
    void inputUpdate(InputData& inputData);
};