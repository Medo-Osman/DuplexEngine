#pragma once
#include "ResourceHandler.h"
#include "Entity.h"
#include "AudioHandler.h"
#include <cmath>


using namespace DirectX;

class ServerPlayer
{
private:
    //NETWORK ID
    int m_playerID;

    //CONTROLLER CONFIG
    const float CAPSULE_HEIGHT = 1.75f;
    const float CAPSULE_RADIUS = 0.5f;

    //WALK CONFIG
    const float PLAYER_SPEED = 10.f;
    DirectX::XMVECTOR m_movementVector;

    //JUMP CONFIG
    const float FALL_MULTIPLIER = 1.1f;
    const float JUMP_DISTANCE = 8.f;
    const float JUMP_SPEED = 20.0f;
    const int ALLOWED_NR_OF_JUMPS = 2;
    int m_jumps;

    //DASH CONFIG 
    const float DASH_TRAVEL_DISTANCE = 15.f;
    const float DASH_SPEED = 10.0f;
    bool m_hasDashed;

    //Roll CONFIG
    const float ROLL_TRAVEL_DISTANCE = 15.f;
    const float ROLL_SPEED = 10.0f;
    const float GRAVITY = 15.f;
    const float ROLL_HEIGHT = 0.3f;
    const float ROLL_RADIUS = 0.2f;


    float m_angleY;
    float m_currentDistance;
    Vector3 m_moveDirection;
 
    Entity* m_playerEntity;
    Transform* m_cameraTransform;

    void handleRotation(const float& dt);
    void playerStateLogic(const float& dt);

    void prepDistVariables();

public:
    ServerPlayer(int ID);

   
    void setPlayerEntity(Entity* entity);

    void setCameraTranformPtr(Transform* transform);

    Entity* getPlayerEntity() const;

    int getNetworkID() { return this->m_playerID; }
    void setNetworkID(int ID) { this->m_playerID = ID; }


    
};
