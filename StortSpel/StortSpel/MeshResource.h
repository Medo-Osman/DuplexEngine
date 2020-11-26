#include "3DPCH.h"
#include "VertexStructs.h"
#include "ReferenceCounted.h"
#pragma once

class MeshResource : public ReferenceCounted
{
private:
	
	Buffer<float> m_vertexBuffer;
	Buffer<std::uint32_t> m_indexBuffer;

	LRM_VERTEX* m_vertexArray = nullptr;

	XMFLOAT3 m_min, m_max;

	std::vector<std::uint32_t> m_materialOffsets;
	
	int refCount = 0;
public:

	int vertCount = 0;
	virtual ~MeshResource()
	{
		//SAFE_DELETE(m_vertexArray);
		m_vertexBuffer.release();
		m_indexBuffer.release();
		delete[] m_vertexArray;
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

		vertCount = nrOfVertecies;
	}

	LRM_VERTEX* getVertexArray()
	{
		return m_vertexArray;
	}

	void setMaterialOffsetsVector(std::uint32_t* materialOffsets, int materialCount)
	{
		for (int i = 0; i < materialCount; i++)
		{
			m_materialOffsets.push_back(materialOffsets[i]);
		}
	}

	int getMaterialCount()
	{
		if (m_materialOffsets.empty())
			return 1;
		else
			return (int)m_materialOffsets.size();
	}
	
	std::pair<std::uint32_t, std::uint32_t> getMaterialOffsetAndSize(int materialIndex)
	{
		if (m_materialOffsets.empty())
		{
			return { 0, m_indexBuffer.getSize() };
		}
		
		int offset;
		int size;

		if (materialIndex == 0)
		{
			offset = 0;
			size = m_materialOffsets.at(materialIndex);
		}
		else
		{
			offset = m_materialOffsets.at(materialIndex - 1);
			size = m_materialOffsets.at(materialIndex) - offset;
		}
		
		return { offset, size };
	}

};