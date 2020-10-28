#pragma once

enum class GUIType { NONE, TEXT, IMAGE, BUTTON };

class GUIElement
{
protected:
	GUIType m_type;
	bool m_visible = true;

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

	bool isVisible()
	{
		return m_visible;
	}

	void setVisible(bool value)
	{
		m_visible = value;
	}

	virtual void render(SpriteBatch* spriteBatch) = 0;
};