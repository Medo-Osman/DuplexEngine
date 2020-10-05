
#pragma once
#include"3DPCH.h"
#include <DirectXColors.h>
using namespace DirectX;

struct PointLightRepresentation
{
    XMVECTOR position;
    XMFLOAT3 color;
    FLOAT intensity;
};

//VertexShader shadowpass constant buffers structs
__declspec(align(16)) struct cbVSWVPMatrix
{
    DirectX::XMMATRIX wvpMatrix;

    void* operator new(size_t i)
    {
        return _mm_malloc(i, 16);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }
};

struct perObjectMVP
{
    XMMATRIX world; //model
    XMMATRIX view;
    XMMATRIX projection;
    XMMATRIX mvpMatrix; //Model/view/projection
};

struct lightBufferStruct
{
    PointLightRepresentation pointLights[8];
    int nrOfPointLights = 0;
};

struct cameraBufferStruct
{
    XMVECTOR cameraPosition;
};

