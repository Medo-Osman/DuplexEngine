#pragma once
#include <DirectXMath.h>
#include"Input_Mouse.h"
#include"Input_Keyboard.h"
#include "ResourceHandler.h"
#include "Entity.h"
#include "Renderer.h"
#include "Camera.h"
using namespace DirectX;

class Player
{
private:
    DirectX::XMVECTOR m_movementVector;
    Entity* m_playerEntity = nullptr;
public:
    Player();

    void updatePlayer(Keyboard* keyboardPtr, const float& dt);
    void setPlayerEntity(Entity* entity);
    Entity* getPlayerEntity();
};