#pragma once
#include <DirectXMath.h>
using namespace DirectX;

enum class ParticleType
{
	EMPTY,
	Emitter,
	Particle
};

struct PositionVertex
{
	XMFLOAT3 position;
	PositionVertex()
	{
		position = {0, 0, 0 };
	}
	PositionVertex(XMFLOAT3 pos)
	{
		position = pos;
	}
};

struct ColorVertex : public PositionVertex
{
	XMFLOAT3 color;
	ColorVertex()
	{
		position = {0.f, 0.f, 0.f };
		color = { 0.f, 0.f, 0.f };
	}
	ColorVertex(XMFLOAT3 pos, XMFLOAT3 clr)
	{
		position = pos;
		color = clr;
	}
};

struct NormalVertex : public PositionVertex
{
	XMFLOAT3 normal;
	NormalVertex()
	{
		this->position = { 0.f, 0.f, 0.f };
		this->normal = { 0.f, 0.f, 0.f };
	}
	NormalVertex(XMFLOAT3 pos, XMFLOAT3 clr)
	{
		this->position = pos;
		this->normal = clr;
	}
};

struct TextureNormalVertex : public NormalVertex
{
	XMFLOAT2 uv;
	TextureNormalVertex()
	{
		this->position = { 0.f, 0.f, 0.f };
		this->normal = { 0.f, 0.f, 0.f };
		this->uv = { 0, 0 };
	}
	TextureNormalVertex(XMFLOAT3 pos, XMFLOAT3 clr, XMFLOAT2 uv)
	{
		this->position = pos;
		this->normal = clr;
		this->uv = uv;
	}
	
};

struct TextureNormalTangentVertex : public TextureNormalVertex
{
	XMFLOAT3 tangent;
	TextureNormalTangentVertex()
	{
		this->position = { 0.f, 0.f, 0.f };
		this->normal = { 0.f, 0.f, 0.f };
		this->uv = { 0, 0 };
		this->tangent = { 0.f, 0.f, 0.f };
	}
	TextureNormalTangentVertex(XMFLOAT3 pos, XMFLOAT3 clr, XMFLOAT2 uv, XMFLOAT3 tangent)
	{
		this->position = pos;
		this->normal = clr;
		this->uv = uv;
		this->tangent = tangent;
	}
};

struct TextureNormalTangentVertexAnim : public TextureNormalTangentVertex
{
	XMFLOAT4 weights;
	BYTE  boneIndex[4];
	
	TextureNormalTangentVertexAnim()
	{
		this->position = { 0.f, 0.f, 0.f };
		this->normal = { 0.f, 0.f, 0.f };
		this->uv = { 0, 0 };
		this->tangent = { 0.f, 0.f, 0.f };
		this->weights = { 0.f, 0.f, 0.f, 0.f };
		this->boneIndex[0] = 0;
		this->boneIndex[1] = 0;
		this->boneIndex[2] = 0;
		this->boneIndex[3] = 0;
	}
	TextureNormalTangentVertexAnim(XMFLOAT3 pos, XMFLOAT3 clr, XMFLOAT2 uv, XMFLOAT3 tangent, XMFLOAT4 weights, XMINT4 boneIndex)
	{
		this->position = pos;
		this->normal = clr;
		this->uv = uv;
		this->tangent = tangent;
		this->weights = weights;
		this->boneIndex[0] = boneIndex.x;
		this->boneIndex[1] = boneIndex.y;
		this->boneIndex[2] = boneIndex.z;
		this->boneIndex[3] = boneIndex.w;
	}
};

struct ParticleVertex
{
	XMFLOAT3 position;
	XMFLOAT3 oldPos;
	XMFLOAT3 velocity;
	XMFLOAT2 size;
	float time;
	float oldTime;
	unsigned int type;

	ParticleVertex()
	{
		this->position = { 0.f, 0.f, 0.f };
		this->oldPos = { 0.f, 0.f, 0.f };
		this->velocity = { 0.f, 0.f, 0.f };
		this->size = { 1, 1 };
		this->time = 0;
		this->oldTime = 0;
		this->type = (unsigned int)ParticleType::Emitter;
	}
	ParticleVertex(XMFLOAT3 pos, XMFLOAT3 oldPos, XMFLOAT3 velocity, XMFLOAT2 size, float time, unsigned int type)
	{
		this->position = pos;
		this->oldPos = oldPos;
		this->velocity = velocity;
		this->size = size;
		this->time = time;
		this->oldTime = time;
		this->type = type;
	}

	
};

struct LRM_VERTEX
{
	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
	XMFLOAT3 bitangent;
	/*LRM_VERTEX(const LRSM_VERTEX& other)
	{
		pos = other.pos;
		texCoord = other.texCoord;
		normal = other.normal;
		tangent = other.tangent;
		bitangent = other.bitangent;
	}*/
};

struct LRSM_VERTEX : public LRM_VERTEX
{
	/*XMFLOAT3 pos;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
	XMFLOAT3 bitangent;*/
	int      boneIdxs[4];
	XMFLOAT4 weights;
};

struct PositionTextureVertex
{
	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
};

struct TEST
{
	XMFLOAT3 pos;
};
