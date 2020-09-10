#pragma once
#include"3DPCH.h"


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
