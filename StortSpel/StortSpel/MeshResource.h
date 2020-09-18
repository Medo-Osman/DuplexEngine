#pragma once
#include "3DPCH.h"

class MeshResource
{
private:
	
	Buffer<LRM_VERTEX> m_vertexBuffer;
	Buffer<std::uint32_t> m_indexBuffer;

public:
	
	~MeshResource()
	{
		m_vertexBuffer.release();
		m_indexBuffer.release();
	}
	
	Buffer<LRM_VERTEX>& getVertexBuffer() { return m_vertexBuffer; }
	Buffer<std::uint32_t>& getIndexBuffer() { return m_indexBuffer; }

	void set(ID3D11DeviceContext* dContext) // ? Ska denna funktionen finnas här och sedan anropas innan draw eller ska koden ligga i en draw funktion 
	{
		UINT offset = 0;
		
		dContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), m_vertexBuffer.getStridePointer(), &offset);
		dContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}
};