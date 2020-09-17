#pragma once
#include"3DPCH.h"

template <typename T>
class Buffer
{
private:
	UINT m_numberOfDataElements;
	UINT m_stride;
	bool m_dynamic;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_bufferPtr;
public:
	Buffer()
	{
		m_numberOfDataElements = 0;
		m_stride = 0;
		m_dynamic = false;
	}
	HRESULT initializeBuffer(ID3D11Device* device, bool dynamic, D3D11_BIND_FLAG bindFlag, T* data, int nrOf, bool defaultUse = false)
	{
		HRESULT hr = 0;
		m_stride = UINT(sizeof(T));
		m_numberOfDataElements = nrOf;
		m_dynamic = dynamic;

		// Buffer Description
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

		bufferDesc.ByteWidth = m_stride * m_numberOfDataElements;
		bufferDesc.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
		if (defaultUse)
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = bindFlag;
		bufferDesc.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE : 0;

		bufferDesc.MiscFlags = 0;
		
		

		if (data != nullptr)
		{
			// Subresource data
			D3D11_SUBRESOURCE_DATA bufferData;
			ZeroMemory(&bufferData, sizeof(D3D11_SUBRESOURCE_DATA));
			bufferData.pSysMem = data;
			bufferData.SysMemPitch = 0;
			bufferData.SysMemSlicePitch = 0;

			hr = device->CreateBuffer(&bufferDesc, &bufferData, m_bufferPtr.GetAddressOf());
			assert(SUCCEEDED(hr) && "Failed to create buffer");
			
		}
		else
		{
			hr = device->CreateBuffer(&bufferDesc, nullptr, m_bufferPtr.GetAddressOf());

		}

		return hr;
	}
	HRESULT updateBuffer(ID3D11DeviceContext* dContext, T* data, int nrOf = 1)
	{
		HRESULT hr;
		if (m_dynamic)
		{
			D3D11_MAPPED_SUBRESOURCE mappedData;
			hr = dContext->Map(m_bufferPtr.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
			if (FAILED(hr))
			{
				OutputDebugString(L"Faied to map buffer\n");
			}
			CopyMemory(mappedData.pData, data, sizeof(T) * nrOf);
			dContext->Unmap(m_bufferPtr.Get(), 0);
		}
		else
		{
			hr = 1;
			OutputDebugString(L"Trying to change non-dynamic buffer.\n");

		}
		return hr;
	}

	void release()
	{
		if(m_bufferPtr != nullptr)
			m_bufferPtr->Release();
	}

	ID3D11Buffer* Get() const { return m_bufferPtr.Get(); }
	ID3D11Buffer* const* GetAddressOf() const { return m_bufferPtr.GetAddressOf(); }

	const UINT getStride() const { return *m_stride; }
	const UINT* getStridePointer() const { return &m_stride; }


	const UINT getSize() const { return m_numberOfDataElements; }


	Microsoft::WRL::ComPtr<ID3D11Buffer> getComPtr()
	{
		return m_bufferPtr;
	}
};