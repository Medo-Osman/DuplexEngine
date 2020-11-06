#pragma once
enum class GUIType { NONE, TEXT, IMAGE, BUTTON };
enum class GUIUpdateType {CLICKED, HOVER_ENTER, HOVER_EXIT};


class GUIElement
{
protected:
	GUIType m_type;
	bool m_visible = true;

public:
	int m_index = 0;
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


class GUIObserver {
public:
	GUIObserver() {};
	virtual void update(GUIUpdateType type, GUIElement* guiElement) = 0;

	static int nr;
	int index = 0;
};

class GUISubject {
public:
	virtual ~GUISubject() {};
	virtual void Attach(GUIObserver* observer) = 0;
	virtual void Detach(GUIObserver* observer) = 0;
	virtual void Notify(GUIUpdateType type) = 0;
};