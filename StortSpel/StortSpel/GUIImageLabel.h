#pragma once

#include "3DPCH.h"
#include "GUIElement.h"
#include "ResourceHandler.h"

struct GUIImageStyle
{
	// Text
	Vector2 position;
	Vector2 origin;
	Vector2 scale;
	float rotation;
	Color color;

	GUIImageStyle()
	{
		position = { 10.f };
		scale = { 1.f };
		origin = { 0.f };
		rotation = 0.f;
		color = Colors::White;
	}
};

class GUIImageLabel : public GUIElement
{
private:
	// Sprite
	GUIImageStyle m_style;

	ID3D11ShaderResourceView* texture = nullptr;

public:
	GUIImageLabel(std::wstring texturePath, GUIImageStyle style)
	{
		m_type = GUIType::IMAGE;
		m_style = style;
	}
	~GUIImageLabel()
	{

	}

	void setTexture(std::wstring texturePath)
	{
		texture = ResourceHandler::get().loadTexture(texturePath.data())->view;
	}

	void render(SpriteBatch* spriteBatch) override
	{
		if (texture)
		{
			spriteBatch->Draw(texture, m_style.position, nullptr, m_style.color, m_style.rotation, m_style.origin, m_style.scale);
		}
	}

};