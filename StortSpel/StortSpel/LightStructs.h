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
    XMVECTOR direction;
    XMFLOAT4 color;
    FLOAT brightness;
};