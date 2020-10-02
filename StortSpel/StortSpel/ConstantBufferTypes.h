
#pragma once
#include"3DPCH.h"
#include <DirectXColors.h>
using namespace DirectX;


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
    XMVECTOR lightPosArray[8] = { 0 };
    XMVECTOR lightColorArray[8] = { 0 };
    int nrOfLights = 0;

    lightBufferStruct& operator=(const lightBufferStruct& other)
    {
        for (int i = 0; i < other.nrOfLights; i++) //Shallow copy
        {
            this->lightColorArray[i] = other.lightColorArray[i];
            this->lightPosArray[i] = other.lightPosArray[i];
            this->nrOfLights = other.nrOfLights;
        }
    }
};

struct cameraBufferStruct
{
    XMVECTOR cameraPosition;
};
