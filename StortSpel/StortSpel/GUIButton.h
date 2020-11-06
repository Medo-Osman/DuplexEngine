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

class GUIButton : public GUIElement, public InputObserver, public GUISubject
{
private:
	// Sprite
	GUIButtonStyle m_style;
	ID3D11ShaderResourceView* texture = nullptr;

	std::map<int, GUIObserver*> m_observers;

	int m_textureWidth = 0;
	int m_textureHeight = 0;

	bool m_hovered = false;

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

		//Messy way to get the texture width and height
		D3D11_TEXTURE2D_DESC desc;
		ID3D11Texture2D* localTexture;
		ID3D11Resource* resource = nullptr;
		texture->GetResource(&resource);//GetDesc(&desc);
		localTexture = static_cast<ID3D11Texture2D*>(resource);
		localTexture->GetDesc(&desc);

		localTexture->Release();
		resource->Release();
		m_textureWidth = desc.Width;
		m_textureHeight = desc.Height;
		//
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
		//Mouse pos
		float x = inputData.mousePtr->getPosX();
		float y = inputData.mousePtr->getPosY();

		//Check if mouse is hovering over button
		if (x > m_style.position.x && x < (m_style.position.x + (m_textureWidth * m_style.scale.x))
			&& y > m_style.position.y && y < (m_style.position.y + (m_textureHeight * m_style.scale.y)) && m_visible)
		{
			//Notify all observers
			if (m_hovered == false)
			{
				//Send message to observers that something has happened
				Notify(GUIUpdateType::HOVER_ENTER);
				m_hovered = true;
			}
		}
		else //When it is not hovering over the button
		{
			if (m_hovered)
			{
				//Send message to observers that something has happened if it was previously hovered over.
				Notify(GUIUpdateType::HOVER_EXIT);
				m_hovered = false;
			}
		}


		auto states = inputData.actionData;
		for (int i = 0; i < states.size(); i++)
		{
			//If clicked left or right mouse button
			if (states[i] == Action::USE)
			{

				//Bounds & visibility check for button vs mouse
				if (x > m_style.position.x && x < (m_style.position.x + (m_textureWidth * m_style.scale.x))
					&& y > m_style.position.y && y < (m_style.position.y + (m_textureHeight * m_style.scale.y)) && m_visible)
				{
					//Notify all observers if clicked
					Notify(GUIUpdateType::CLICKED);
				}

				
			}
		}

	}


	// Inherited via GUISubject
	virtual void Attach(GUIObserver* observer) override
	{
		m_observers[GUIObserver::nr++] = observer;
	}

	virtual void Detach(GUIObserver* observer) override
	{
		m_observers.erase(observer->index);
	}

	virtual void Notify(GUIUpdateType type) override
	{
		for (auto observer : m_observers)
		{
			observer.second->update(type, this);
		}
	}

};