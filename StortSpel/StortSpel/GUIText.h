#pragma once

#include "3DPCH.h"
#include "GUIElement.h"

struct GUITextStyle
{
	// Text
	Vector2 position;
	Vector2 origin;
	Vector2 scale;
	float rotation;
	Color color;

	// Outline
	bool hasOutline;
	float offsetPos;
	Color outlineColor;

	GUITextStyle()
	{
		position = { 10.f };
		scale = { 1.f };
		origin = { 0.f };
		rotation = 0.f;
		color = Colors::White;

		hasOutline = true;
		offsetPos = 1.f;
		outlineColor = Colors::Black;
	}
};

class GUIText : public GUIElement
{
private:
	// Sprite
	SpriteFont* m_spriteFont;
	std::string m_textString;

	// Style
	GUITextStyle m_style;

public:
	GUIText()
	{
		m_type = GUIType::TEXT;

		m_spriteFont = nullptr;
		m_textString = "NULL";
	}
	~GUIText()
	{

	}

	void setText(std::string newText, SpriteFont* spriteFont, GUITextStyle style = GUITextStyle())
	{
		m_textString = newText;
		m_spriteFont = spriteFont;
		m_style = style;
	}

	void changeText(std::string newText)
	{
		m_textString = newText;
	}

	void render(SpriteBatch* spriteBatch) override
	{
		//Outline
		if (m_style.hasOutline)
		{
			m_spriteFont->DrawString(spriteBatch, m_textString.c_str(), { m_style.position.x + m_style.offsetPos, m_style.position.y + m_style.offsetPos }, m_style.outlineColor, m_style.rotation, m_style.origin, m_style.scale);
			m_spriteFont->DrawString(spriteBatch, m_textString.c_str(), { m_style.position.x - m_style.offsetPos, m_style.position.y + m_style.offsetPos }, m_style.outlineColor, m_style.rotation, m_style.origin, m_style.scale);
			m_spriteFont->DrawString(spriteBatch, m_textString.c_str(), { m_style.position.x - m_style.offsetPos, m_style.position.y - m_style.offsetPos }, m_style.outlineColor, m_style.rotation, m_style.origin, m_style.scale);
			m_spriteFont->DrawString(spriteBatch, m_textString.c_str(), { m_style.position.x + m_style.offsetPos, m_style.position.y - m_style.offsetPos }, m_style.outlineColor, m_style.rotation, m_style.origin, m_style.scale);
		}

		//Text
		m_spriteFont->DrawString(spriteBatch, m_textString.c_str(), m_style.position, m_style.color, m_style.rotation, m_style.origin, m_style.scale);
	}

	void setStyle(GUITextStyle style)
	{
		m_style = style;
	}

};