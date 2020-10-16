#include "3DPCH.h"
#pragma once



class MeshResource
{
private:
	
	Buffer<float> m_vertexBuffer;
	Buffer<std::uint32_t> m_indexBuffer;

	LRM_VERTEX* m_vertexArray = nullptr;

	XMFLOAT3 m_min, m_max;
	
public:
	~MeshResource()
	{
		SAFE_DELETE(m_vertexArray);
		//m_vertexBuffer.release();
		//m_indexBuffer.release();
	}
	
	Buffer<float>& getVertexBuffer() { return m_vertexBuffer; }
	Buffer<std::uint32_t>& getIndexBuffer() { return m_indexBuffer; }

	void set(ID3D11DeviceContext* dContext) // ? Ska denna funktionen finnas här och sedan anropas innan draw eller ska koden ligga i en draw funktion 
	{
		UINT offset = 0;
		
		dContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), m_vertexBuffer.getStridePointer(), &offset);
		dContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}

	void setMinMax(const XMFLOAT3& min, const XMFLOAT3& max)
	{
		m_min = min;
		m_max = max;
	}

	void getMinMax(XMFLOAT3& min, XMFLOAT3& max)
	{
		min = m_min;
		max = m_max;
	}

	void storeVertexArray(LRM_VERTEX vertexArray[], int nrOfVertecies)
	{
		m_vertexArray = new LRM_VERTEX[nrOfVertecies];
		for (int i = 0; i < nrOfVertecies; i++)
		{
			m_vertexArray[i] = vertexArray[i];
		}
	}

	LRM_VERTEX* getVertexArray()
	{
		return m_vertexArray;
	}
};