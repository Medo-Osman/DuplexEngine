#pragma once
#include "Component.h"
#include "Transform.h"

enum LightType{ Point, Spot };

class LightComponent : public Component, public Transform
{
private:
	std::string m_lightID;
	XMFLOAT3 m_color;
	FLOAT m_intensity = 1.f;
	LightType m_lightType;

	void init(Vector3 position = Vector3(), XMFLOAT3 color = XMFLOAT3(1, 1, 1), FLOAT intensity = 1.f)
	{
		m_lightType = LightType::Point;

		setPosition(position);
		m_color = color;

		m_intensity = intensity;

		m_type = ComponentType::LIGHT;
	}

public:

	LightComponent(Vector3 position = Vector3(), XMFLOAT3 color = XMFLOAT3(1, 1, 1), FLOAT intensity = 1.f)
	{
		init(position, color, intensity);
	}

	LightComponent(char* paramData)
	{
		// Read data from package
		int offset = sizeof(LightType);

		Vector3 position = readDataFromChar<Vector3>(paramData, offset);
		Vector3 color = readDataFromChar<Vector3>(paramData, offset);
		float intensity = readDataFromChar<float>(paramData, offset);

		init(position, color, intensity);
	}
	
	// Update
	virtual void update(float dt) override {}

	void setLightID(std::string id)
	{
		m_lightID = id;
	}

	std::string getLightID()
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