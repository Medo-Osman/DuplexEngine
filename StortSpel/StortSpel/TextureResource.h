#pragma once
#include "3DPCH.h"
#include "ReferenceCounted.h"

class TextureResource : public ReferenceCounted
{
public:
	ID3D11ShaderResourceView* view = nullptr;

	~TextureResource()
	{
		if (view)
		{
			//int nr = view->Release();
			//int test = 1;
		}
	}
};