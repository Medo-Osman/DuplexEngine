#pragma once
#include "Component.h"
#include "Transform.h"


class PointLightComponent : public Component, public Transform
{
private:
	UINT32 m_lightID = 0;
	Color m_color;
public:
	PointLightComponent(Vector3 position = Vector3(), Color color = Color(1, 1, 1))
	{
		translation(position);
		m_color = color;

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

	void setColor(Color color)
	{
		m_color = color;
	}

	Color getColor()
	{
		return m_color;
	}

};