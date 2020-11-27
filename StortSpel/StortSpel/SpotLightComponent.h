#pragma once
#include "Component.h"
#include "Transform.h"
#include "LightComponent.h"


class SpotLightComponent : public LightComponent
{
private:

	XMFLOAT3 m_direction;
	FLOAT m_coneFactor = 8.f; //Smaller value means larger cone of light, 8 = ~45 degrees

	void init(Vector3 position = Vector3(), XMFLOAT3 color = XMFLOAT3(1, 1, 1), FLOAT intensity = 1.f, XMFLOAT3 direction = XMFLOAT3(0, 0, 1))
	{
		setPosition(position);
		setColor(color);
		setIntensity(intensity);
		m_direction = direction;
		m_type = ComponentType::LIGHT;

		setLightType(LightType::Spot);
	}

public:
	SpotLightComponent(Vector3 position = Vector3(), XMFLOAT3 color = XMFLOAT3(1, 1, 1), FLOAT intensity = 1.f, XMFLOAT3 direction = XMFLOAT3(0,0,1))
	{
		init(position, color, intensity, direction);
	}

	SpotLightComponent(char* paramData)
	{
		// Read data from package
		int offset = sizeof(LightType);

		Vector3 position = readDataFromChar<Vector3>(paramData, offset);
		Vector3 color = readDataFromChar<Vector3>(paramData, offset);
		float intensity = readDataFromChar<float>(paramData, offset);
		
		Vector4 rotation = readDataFromChar<Vector4>(paramData, offset);
		Vector3 direction = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), rotation);

		init(position, color, intensity, direction);
	}

	void setDirection(XMFLOAT3 direction)
	{
		m_direction = direction;
	}

	XMFLOAT3 getDirection()
	{
		return m_direction;
	}

	void setConeFactor(float factor)
	{
		m_coneFactor = factor;
	}

	FLOAT getConeFactor()
	{
		return m_coneFactor;
	}
};