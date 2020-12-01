#pragma once
#include "3DPCH.h"
#include "ReferenceCounted.h"

class TextureResource : public ReferenceCounted
{
public:
	ID3D11ShaderResourceView* view = nullptr;
	bool m_doReferenceCount = false;

	TextureResource()
	{
		std::cout << "Texture resource, " << this << std::endl;
	}

	~TextureResource()
	{
		if (view)
		{
			int nr = view->Release();
			std::cout << "\tTexture resource release, nr: " << nr << ", " << this << std::endl;
			view = nullptr;
		}
		
	}

	void Release()
	{
		if (view)
		{
			int nr = view->Release();
			std::cout << "\tTexture resource release, nr: " << nr << std::endl;
			view = nullptr;
		}
		else
		{
			ErrorLogger::get().logError(L"Trying to release released texture resource");
		}
	}
};
