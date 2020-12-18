
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
    FLOAT ambientLightLevel = 0.3f;
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

struct globalConstBuffer
{
    Vector3 playerPosition = { 0.0f, 0.0f, 0.0f };
    float environmentMapBrightness = 1.0f;
    float time = 1.0f;
    float globalTimeDilation = 1.0f;
    Vector2 padding;
};

struct cloudConstBuffer
{
    float cloudBedHeightPosition = 0.0f;
    float cloudDisplacementFactor = 100.0f;
    float cloudTessellationFactor = 14.99f;
    float worleyScale = 0.02f;
    Vector3 panSpeed = { 0.032f, -0.026f, -0.009f };
    float tile = 1.0f;
    float dispPower = 0.1f;
    float occlusionFactor = 1.0f;
    float backlightFactor = 3.0f;
    float backlightStrength = 1.0f;
    Vector3 backlightColor = { 1.0f, 0.156f, 0.024f };
    float preDisplacement = -15.0f;
};

struct atmosphericFogConstBuffer
{
    Vector3 FogColor = { 1.0f, 1.0f, 1.0f };
    float FogStartDepth = 200.0;
    Vector3 FogHighlightColor = { 1.0f, 1.0f, 1.0f };
    float FogGlobalDensity = 0.009f;
    Vector3 FogSunDir = { 1.0f, 0.0f, 0.0f };
    float FogHeightFalloff = 0.169f;
    float FogStartDepthSkybox = 14.0;

    float cloudFogHeightStart = -5.0f;
    float cloudFogHeightEnd = 160.0f;
    float cloudFogStrength = 0.25f;
    Vector3 cloudFogColor = { 1.0f, 1.0f, 1.0f };
    float padding;
};

struct SSAO_BUFFER
{
    XMMATRIX viewToTexSpace;
    XMMATRIX worldInverseTransposeView;
    XMFLOAT4 offsetVectors[14];
    XMFLOAT4 frustumCorners[4];

    float occlusionRadius = 0.5f;
    float occlusionFadeStart = 0.2f;
    float occlusionFadeEnd = 2.0f;
    float surfaceEpsilon = 0.05f;
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