#pragma once

#include "GUIElement.h"
#include "GUIText.h"
#include "GUIImageLabel.h"
#include "GUIButton.h"
#include "Input.h"

class GUIHandler : public InputObserver
{
private:
	GUIHandler();

	// Device
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_dContext;
	HWND* m_window;
	// Render States
	std::unique_ptr< CommonStates > m_states;

	// Screen Dimensions
	int m_screenWidth;
	int m_screenHeight;

	// SpriteBatch
	SpriteBatch* m_spriteBatch;

	// GUI Elements
	std::vector< GUIElement* > m_elements;
	std::unordered_map< std::wstring, SpriteFont* > m_fonts;
	const std::wstring m_FONTS_PATH = L"../res/fonts/";

	Input* m_input = nullptr;

	// Menu Gamepad Selection
	int m_inMenu;
	int m_selectedMenuButton;

public:
	static GUIHandler& get();
	~GUIHandler();

	void initialize(ID3D11Device* device, ID3D11DeviceContext* dContext, Input* input, HWND* window);

	int addGUIButton(std::wstring buttonTextureString, GUIButtonStyle style = GUIButtonStyle());
	void changeGUIButton(int index, std::wstring path);

	int addGUIText(std::string textString, std::wstring fontName, GUITextStyle style = GUITextStyle());
	void changeGUIText(int index, std::string newTextString);
	
	int addGUIImage(std::wstring textureString, GUIImageStyle style = GUIImageStyle());
	void changeGUIImage(int index, std::wstring path);
	void removeElement(int index);

	void setVisible(int index, bool value);
	bool getVisible(int index);

	void setImageStyle(int index, GUIImageStyle style);
	void setGUITextStyle(int index, GUITextStyle style);

	std::vector< GUIElement* >* getElementMap();

	// Menu Gamepad Selection
	void setInMenu(bool inMenu, int startIndex = 0);

	void render();

	virtual void inputUpdate(InputData& inputData) override;
};