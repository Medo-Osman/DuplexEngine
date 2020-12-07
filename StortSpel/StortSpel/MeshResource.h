#include "3DPCH.h"
#include "VertexStructs.h"
#include "ReferenceCounted.h"
#pragma once

class MeshResource : public ReferenceCounted
{
private:
	
	Buffer<float> m_vertexBuffer;
	Buffer<std::uint32_t> m_indexBuffer;
	std::string m_filePath;

	PositionVertex* m_vertexArray = nullptr;
	std::uint32_t* m_indexArray = nullptr;

	XMFLOAT3 m_min, m_max;
	XMFLOAT3 m_boundsCenter;

	std::vector<std::uint32_t> m_materialOffsets;
	
	int refCount = 0;
public:
	int m_vertCount = 0;
	int m_indexCount = 0;
	virtual ~MeshResource()
	{
		//SAFE_DELETE(m_vertexArray);
		std::cout << "DELETE MESH RES " << debugName << std::endl;

		m_vertexBuffer.release();
		m_indexBuffer.release();

		if (m_indexArray)
		{
			delete[] m_indexArray;

		}

		if (m_vertexArray)
		{
			delete[] m_vertexArray;

		}
	}
	
	Buffer<float>& getVertexBuffer() { return m_vertexBuffer; }
	Buffer<std::uint32_t>& getIndexBuffer() { return m_indexBuffer; }

	void set(ID3D11DeviceContext* dContext)
	{
		UINT offset = 0;
		
		dContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), m_vertexBuffer.getStridePointer(), &offset);
		dContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}

	void setMinMax(const XMFLOAT3& min, const XMFLOAT3& max)
	{
		m_min = min;
		m_max = max;
		m_boundsCenter = (m_max + m_min) / 2.f ;
	}

	void getMinMax(XMFLOAT3& min, XMFLOAT3& max)
	{
		min = m_min;
		max = m_max;
	}

	void getBoundsCenter(XMFLOAT3& boundsCenter)
	{
		boundsCenter = m_boundsCenter;
	}

	XMFLOAT3 getBoundsCenter()
	{
		return m_boundsCenter;
	}

	void storeVertexArray(LRM_VERTEX vertexArray[], int nrOfVertecies)
	{
		//m_vertexArray = new PositionVertex[nrOfVertecies];
		//for (int i = 0; i < nrOfVertecies; i++)
		//{
		//	m_vertexArray[i].position = vertexArray[i].pos;
		//}

		//m_vertCount = nrOfVertecies;
	}

	void storeIndexArray(std::uint32_t indexBuffer[], int nrOfIndicies)
	{
		//m_indexArray = new std::uint32_t[nrOfIndicies];
		//for (int i = 0; i < nrOfIndicies; i++)
		//{
		//	m_indexArray[i] = indexBuffer[i];
		//}

		//m_indexCount = nrOfIndicies;
	}

	void storeFilePath(std::string name)
	{
		m_filePath = name;
	}

	const std::string& getFilePath()
	{
		return m_filePath;
	}

	int getVertexArraySize()
	{
		return m_vertCount;
	}

	int getIndexArraySize()
	{
		return m_indexCount;
	}

	PositionVertex* getVertexArray()
	{
		return m_vertexArray;
	}

	uint32_t* getIndexArray()
	{
		return m_indexArray;
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