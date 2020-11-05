#pragma once

#include "3DPCH.h"
#include "GUIElement.h"
#include "ResourceHandler.h"
#include "Input.h"

struct GUIButtonStyle
{
	// Text
	Vector2 position;
	Vector2 origin;
	Vector2 scale;
	float rotation;
	Color color;

	GUIButtonStyle()
	{
		position = { 10.f };
		scale = { 1.f };
		origin = { 0.f };
		rotation = 0.f;
		color = Colors::White;
	}
};

class GUIButton : public GUIElement, public InputObserver
{
private:
	// Sprite
	GUIButtonStyle m_style;

	ID3D11ShaderResourceView* texture = nullptr;

public:
	GUIButton(std::wstring texturePath, GUIButtonStyle style)
	{
		m_type = GUIType::BUTTON;
		m_style = style;
		//ApplicationLayer::getInstance().m_input.Attach(this);
	}
	~GUIButton()
	{

	}

	void setTexture(std::wstring texturePath)
	{
		texture = ResourceHandler::get().loadTexture(texturePath.data());
	}

	void render(SpriteBatch* spriteBatch) override
	{
		if (texture)
		{
			spriteBatch->Draw(texture, m_style.position, nullptr, m_style.color, m_style.rotation, m_style.origin, m_style.scale);
		}
	}

	/*
	if (inputData.rangeData[i].rangeFlag == Range::RAW)
		{
			if (!m_newIncrements)
			{
				m_incrementRotation = XMVectorZero();
				m_newIncrements = true;
			}

			XMFLOAT2 mouseDelta = XMFLOAT2((float)inputData.rangeData[i].pos.x, (float)inputData.rangeData[i].pos.y);
	*/

	// Inherited via InputObserver
	virtual void inputUpdate(InputData& inputData) override
	{

		float x = 0;
		float y = 0;
		for(int k = 0; k < inputData.rangeData.size(); k++)
		{
			if(inputData.rangeData[k].rangeFlag == Range::REL)
			{
				x = (float)inputData.rangeData[k].pos.x;
				y = (float)inputData.rangeData[k].pos.y;

				std::cout << x << ", " << y << std::endl;
			}
		}

		auto states = inputData.actionData;
		for (int i = 0; i < states.size(); i++)
		{
			if (states[i] == Action::USE)
			{
				//Input::getMouse
				std::cout << "WE CLICKED YEEHAW" << std::endl;
				
				
				std::cout << std::to_string(inputData.mousePtr->getPosx());
				for(int k = 0; k < inputData.rangeData.size(); k++)
				{
					if(inputData.rangeData[k].rangeFlag == Range::REL)
					{
						float x = (float)inputData.rangeData[k].pos.x;
						float y = (float)inputData.rangeData[k].pos.y;

						std::cout << x << ", " << y << std::endl;
					}
				}
				
			}
		}

	}

};