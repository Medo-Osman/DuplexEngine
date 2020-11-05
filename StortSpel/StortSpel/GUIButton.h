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


		auto states = inputData.actionData;
		for (int i = 0; i < states.size(); i++)
		{
			if (states[i] == Action::USE)
			{
				
				float x = inputData.mousePtr->getPosx();
				float y = inputData.mousePtr->getPosY();

				
				D3D11_TEXTURE2D_DESC desc;
				ID3D11Texture2D* localTexture;
				ID3D11Resource* resource = nullptr;
				texture->GetResource(&resource);//GetDesc(&desc);
				localTexture = static_cast<ID3D11Texture2D*>(resource);
				localTexture->GetDesc(&desc);

				localTexture->Release();
				resource->Release();


				if (x > m_style.origin.x && x < (m_style.origin.x + (desc.Width * m_style.scale.x))
					&& y > m_style.origin.y && y < (m_style.origin.y + (desc.Height * m_style.scale.y)))
				{

					std::cout << "WE CLICKED YEEHAW" << std::endl;
					std::cout << x << ", " << y << "\t vs \t" << desc.Width * m_style.origin.x + (desc.Width * m_style.scale.x) << ", " << m_style.origin.y + (desc.Height * m_style.scale.y) << std::endl;
				}
				//Input::getMouse

				
			}
		}

	}

};