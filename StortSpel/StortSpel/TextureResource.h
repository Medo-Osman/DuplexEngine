#pragma once
#include "3DPCH.h"
#include "ReferenceCounted.h"

class TextureResource : public ReferenceCounted
{
public:
	ID3D11ShaderResourceView* view = nullptr;
	bool m_doReferenceCount = false;

	~TextureResource()
	{
		if (view)
		{
			int nr = view->Release();
			view = nullptr;
			std::cout << "refcount " << nr << std::endl;
		}
	}
};