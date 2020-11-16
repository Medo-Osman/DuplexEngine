
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
