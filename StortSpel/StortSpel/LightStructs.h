#pragma once
#include"3DPCH.h"

struct PointLightRepresentation
{
    XMVECTOR position;
    XMFLOAT3 color;
    FLOAT intensity;
};

struct SpotLightRepresentation
{
    XMVECTOR position;
    XMFLOAT3 color;
    FLOAT intensity;
    XMFLOAT3 direction;
    FLOAT coneFactor;
};

struct DirectionLight
{
    XMVECTOR direction = { -0.767f, -0.574f, 0.382f};
    XMFLOAT4 color = { 1.0f, 0.156f, 0.024f, 1.0f };
    FLOAT brightness = 15.0f;
};