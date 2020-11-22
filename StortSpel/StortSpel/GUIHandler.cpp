#include "3DPCH.h"
#include "GUIHandler.h"

int GUIObserver::nr = 0;
GUIHandler::GUIHandler()
{
	m_device = nullptr;
	m_dContext = nullptr;
	m_spriteBatch = nullptr;
	m_screenWidth = 0;
	m_screenHeight = 0;
}

GUIHandler& GUIHandler::get()
{
	static GUIHandler instance;
	return instance;
}

GUIHandler::~GUIHandler()
{
	for (std::pair< std::wstring, SpriteFont* > font : m_fonts)
		delete font.second;

	m_fonts.clear();

	for (int i = 0; i < m_elements.size(); i++)
		delete m_elements[i];

	delete m_spriteBatch;
}

void GUIHandler::initialize(ID3D11Device* device, ID3D11DeviceContext* dContext, Input* input, HWND* window)
{
	m_device = device;
	m_dContext = dContext;
	m_window = window;
	m_input = input;

	m_states = std::make_unique< CommonStates >(m_device);

	m_spriteBatch = new SpriteBatch(m_dContext);
}

int GUIHandler::addGUIButton(std::wstring buttonTextureString, GUIButtonStyle style)
{
	int index = (int)m_elements.size();
	GUIButton* image = new GUIButton(buttonTextureString, style, m_window);
	image->m_index = index;
	image->setTexture(buttonTextureString);
	m_elements.push_back(image);

	m_input->Attach(image);

	return index;
}

void GUIHandler::changeGUIButton(int index, std::wstring path)
{
	static_cast<GUIButton*>(m_elements[index])->setTexture(path);
}

int GUIHandler::addGUIText(std::string textString, std::wstring fontName, GUITextStyle style)
{
	// Font L"squirk.spritefont"
	if (m_fonts.find(fontName) == m_fonts.end())
	{
		std::wstring path = m_FONTS_PATH + fontName;
		m_fonts[fontName] = new SpriteFont(m_device, path.c_str());
	}

	// Text Element
	int index = (int)m_elements.size();
	GUIText* text = new GUIText();
	text->m_index = index;
	text->setText(textString, m_fonts[fontName], style);
	m_elements.push_back(text);

	return index;
}


void GUIHandler::changeGUIText(int index, std::string newTextString)
{
	//To-do: remove unused texture from resourceHandler
	static_cast<GUIText*>(m_elements[index])->changeText(newTextString);
}

int GUIHandler::addGUIImage(std::wstring textureString, GUIImageStyle style)
{
	// Gui Element
	int index = (int)m_elements.size();
	GUIImageLabel* image = new GUIImageLabel(textureString, style);
	image->m_index = index;
	image->setTexture(textureString);
	m_elements.push_back(image);

	return index;
}

void GUIHandler::changeGUIImage(int index, std::wstring path)
{
	static_cast<GUIImageLabel*>(m_elements[index])->setTexture(path);
}

void GUIHandler::removeElement(int index)
{
	m_elements.erase(m_elements.begin() + index);
}

void GUIHandler::setVisible(int index, bool value)
{
	return (m_elements[index]->setVisible(value));
}

bool GUIHandler::getVisible(int index)
{
	return (m_elements[index]->isVisible());
}

std::vector<GUIElement*>* GUIHandler::getElementMap()
{
	return &this->m_elements;
}

void GUIHandler::render()
{
	m_spriteBatch->Begin(SpriteSortMode_Deferred, m_states->NonPremultiplied());

	for (size_t i = 0; i < m_elements.size(); i++)
	{
		if (m_elements[i]->isVisible())
			m_elements[i]->render(m_spriteBatch);
	}
		

	m_spriteBatch->End();
}