
#pragma once
#include "3DPCH.h"
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


__declspec(align(16)) struct LineData
{
    XMFLOAT3 position;
    float pad;
    XMFLOAT3 direction;
    float paddy;

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
    FLOAT ambientLightLevel = 0.1f;
    PointLightRepresentation pointLights[8];
    int nrOfPointLights = 0;

    SpotLightRepresentation spotLights[8];
    int nrOfSpotLights = 0;

    DirectionLight skyLight;
};

struct cameraBufferStruct
{
    XMVECTOR cameraPosition;
};

struct skyboxMVP
{
    XMMATRIX mvpMatrix; //Model/view/projection
};

struct shadowBuffer
{
    XMMATRIX lightViewMatrix;
    XMMATRIX lightProjMatrix;
    XMMATRIX shadowMatrix;
};

const int MAX_JOINT_COUNT = 30; // if this is changed, also change it in the shader.

struct skeletonAnimationCBuffer
{
    XMMATRIX boneMatrixPallet[MAX_JOINT_COUNT]; // 30 is currently the max number of joint, this can be changed later if needed
};

const int BLUR_RADIUS = 15;

struct CS_BLUR_CBUFFER
{
    float weights[BLUR_RADIUS + 1]; // BLUR_RADIUS + 1 * 4 bytes
    int radius;
    int direction;
    XMFLOAT2 pad;
};

__declspec(align(16)) struct projectionMatrix
{
    XMMATRIX g_viewProjectionMatrix;
    XMFLOAT3 g_cameraPos;
    int pad;
    void* operator new(size_t i)
    {
        return _mm_malloc(i, 16);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }
};
__declspec(align(16)) struct acceleration
{
    XMFLOAT3 worldAcceleration;
    int pad;

    void* operator new(size_t i)
    {
        return _mm_malloc(i, 16);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }
};
__declspec(align(16)) struct ParticleDataVS //VS
{
    XMVECTOR worldEmitPosition;
    float gameTime;
    float dt;
    float intensity; //1 - 100
    int pad;

    void* operator new(size_t i)
    {
        return _mm_malloc(i, 16);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }
};