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

	HWND* m_window;

	int m_textureWidth = 0;
	int m_textureHeight = 0;

	bool m_hovered = false;
	bool m_gamepadSelected = false;

	// Menu Gamepad Selection
	GUIButton* m_nextMenuButton = nullptr;
	GUIButton* m_prevMenuButton = nullptr;

public:
	GUIButton(std::wstring texturePath, GUIButtonStyle style, HWND* window)
	{
		m_type = GUIType::BUTTON;
		m_style = style;
		m_window = window;
		//ApplicationLayer::getInstance().m_input.Attach(this);
	}
	~GUIButton()
	{

	}

	int getNextMenuButton() { return m_nextMenuButton->m_index; }
	int getPrevMenuButton() { return m_prevMenuButton->m_index; }
	void setPrevMenuButton(GUIButton* prevButton) { m_prevMenuButton = prevButton; }
	void setNextMenuButton(GUIButton* nextButton) { m_nextMenuButton = nextButton; }

	void setIsHovered(bool hovered)
	{
		m_hovered = hovered;
	}

	void setIsSelected(bool selected)
	{
		m_gamepadSelected = selected;
		if (m_gamepadSelected)
			Notify(GUIUpdateType::HOVER_ENTER);
		else
			Notify(GUIUpdateType::HOVER_EXIT);
	}

	void setTexture(std::wstring texturePath)
	{
		texture = ResourceHandler::get().loadTexture(texturePath.data())->view;

		//Messy way to get the texture width and height
		D3D11_TEXTURE2D_DESC desc;
		ID3D11Texture2D* localTexture;
		ID3D11Resource* resource = nullptr;
		texture->GetResource(&resource);//GetDesc(&desc);
		localTexture = static_cast<ID3D11Texture2D*>(resource);
		localTexture->GetDesc(&desc);

		localTexture->Release();
		//resource->Release();
		m_textureWidth = desc.Width;
		m_textureHeight = desc.Height;
		//
	}

	void setStyle(GUIButtonStyle newStyle)
	{
		m_style = newStyle;
	}

	void render(SpriteBatch* spriteBatch) override
	{
		if (texture)
		{
			spriteBatch->Draw(texture, m_style.position, nullptr, m_style.color, m_style.rotation, m_style.origin, m_style.scale);
		}
	}

	// Inherited via InputObserver
	virtual void inputUpdate(InputData& inputData) override
	{
		//Mouse pos
		//float x = inputData.mousePtr->getPosX();
		//float y = inputData.mousePtr->getPosY();
		POINT p;
		float x;
		float y;
		if (GetCursorPos(&p))
		{
			//cursor position now in p.x and p.y
		}
		if (ScreenToClient(*m_window, &p))
		{
			//p.x and p.y are now relative to hwnd's client area
			x = (float)p.x;
			y = (float)p.y;
			
		}

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
			if (!m_gamepadSelected)
			{
				if (m_hovered)
				{
					//Send message to observers that something has happened if it was previously hovered over.
					Notify(GUIUpdateType::HOVER_EXIT);
					m_hovered = false;
				}
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

			if (states[i] == Action::SELECT)
			{
				if(m_gamepadSelected)
					Notify(GUIUpdateType::CLICKED);
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