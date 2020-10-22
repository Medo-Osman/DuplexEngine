#pragma once

enum class GUIType { NONE, TEXT, IMAGE, BUTTON };

class GUIElement
{
protected:
	GUIType m_type;

public:
	GUIElement()
	{
		m_type = GUIType::NONE;
	}
	virtual ~GUIElement() {}

	GUIType getType()
	{
		return m_type;
	}

	virtual void render(SpriteBatch* spriteBatch) = 0;
};