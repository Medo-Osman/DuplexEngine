#pragma once
#include "Component.h"
#include "Transform.h"

enum LightType{ Point, Spot };

class LightComponent : public Component, public Transform
{
private:
	UINT32 m_lightID = 0;
	XMFLOAT3 m_color;
	FLOAT m_intensity = 1.f;
	LightType m_lightType;
public:

	LightComponent(Vector3 position = Vector3(), XMFLOAT3 color = XMFLOAT3(1, 1, 1), FLOAT intensity = 1.f)
	{
		m_lightType = LightType::Point;

		translation(position);
		m_color = color;

		m_intensity = intensity;

		m_type = ComponentType::LIGHT;
	}
	
	// Update
	virtual void update(float dt) override {}

	void setLightID(UINT32 id)
	{
		m_lightID = id;
	}

	UINT32 getLightID()
	{
		return m_lightID;
	}

	void setColor(XMFLOAT3 color)
	{
		m_color = color;
	}

	XMFLOAT3 getColor()
	{
		return m_color;
	}

	void setIntensity(FLOAT intensity)
	{
		m_intensity = intensity;
	}

	FLOAT getIntensity()
	{
		return m_intensity;
	}

	void setLightType(LightType type)
	{
		m_lightType = type;
	}

	LightType getLightType()
	{
		return m_lightType;
	}

};