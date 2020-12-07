#pragma once
#include"3DPCH.h"
#include <mutex>

template <typename T>
class Buffer
{
private:
	UINT m_numberOfDataElements;
	UINT m_stride;
	bool m_dynamic;
	ID3D11Buffer* m_bufferPtr;

	std::mutex m_lock;
public:
	Buffer()
	{
		m_numberOfDataElements = 0;
		m_stride = 0;
		m_dynamic = false;
	}

	~Buffer()
	{
		//if (m_bufferPtr != nullptr)
		//{
			//m_bufferPtr.ReleaseAndGetAddressOf();

			//int nr = m_bufferPtr->Release();
			//m_bufferPtr = nullptr;
			//std::cout << "rel: " << nr << std::endl;
		//}
	}

	HRESULT initializeBuffer(ID3D11Device* device, bool dynamic, D3D11_BIND_FLAG bindFlag, T* data, int nrOf, bool defaultUse = false, UINT stride = 0)
	{
		HRESULT hr = 0;
		
		if (stride == 0)
			m_stride = UINT(sizeof(T));
		else
			m_stride = stride;

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

			hr = device->CreateBuffer(&bufferDesc, &bufferData, &m_bufferPtr);
			assert(SUCCEEDED(hr) && "Failed to create buffer");
			
		}
		else
		{
			hr = device->CreateBuffer(&bufferDesc, nullptr, &m_bufferPtr);

		}
		return hr;
	}
	HRESULT updateBuffer(ID3D11DeviceContext* dContext, T* data, int nrOf = 1)
	{

		HRESULT hr;
		if (m_dynamic)
		{
			D3D11_MAPPED_SUBRESOURCE mappedData;
			hr = dContext->Map(m_bufferPtr, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
			if (FAILED(hr))
			{
				OutputDebugString(L"Faied to map buffer\n");
			}
			CopyMemory(mappedData.pData, data, sizeof(T) * nrOf);
			dContext->Unmap(m_bufferPtr, 0);
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
		//if (m_bufferPtr != nullptr)
		//{
			//m_bufferPtr.ReleaseAndGetAddressOf();
			int nr = m_bufferPtr->Release();
			//std::cout << "refcount: " << nr << std::endl;
			m_bufferPtr = nullptr;
		//}

	}

	ID3D11Buffer* Get() const { return m_bufferPtr; }
	ID3D11Buffer* const* GetAddressOf() const { return &m_bufferPtr; }

	const UINT getStride() const { return *m_stride; }
	const UINT* getStridePointer() const { return &m_stride; }


	const UINT getSize() const { return m_numberOfDataElements; }


	Microsoft::WRL::ComPtr<ID3D11Buffer> getComPtr()
	{
		return m_bufferPtr;
	}
};