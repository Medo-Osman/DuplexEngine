#include "3DPCH.h"
#include "GUIHandler.h"

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

	delete m_spriteBatch;
}

void GUIHandler::initialize(ID3D11Device* device, ID3D11DeviceContext* dContext)
{
	m_device = device;
	m_dContext = dContext;

	m_states = std::make_unique< CommonStates >(m_device);

	m_spriteBatch = new SpriteBatch(m_dContext);
}

int GUIHandler::addGUIText(std::string textString, std::wstring fontName, GUITextStyle style)
{
	// Font L"squirk.spritefont"
	if (!m_fonts.count(fontName))
	{
		std::wstring path = m_FONTS_PATH + fontName;
		m_fonts[fontName] = new SpriteFont(m_device, path.c_str());
	}

	// Text Element
	int index = m_elements.size();
	GUIText* text = new GUIText();
	text->setText(textString, m_fonts[fontName], style);
	m_elements.push_back(text);

	return index;
}

void GUIHandler::changeGUIText(int index, std::string newTextString)
{
	static_cast<GUIText*>(m_elements[index])->changeText(newTextString);
}

void GUIHandler::removeElement(int index)
{
	m_elements.erase(m_elements.begin() + index);
}

void GUIHandler::render()
{
	m_spriteBatch->Begin(SpriteSortMode_Deferred, m_states->NonPremultiplied());

	for (size_t i = 0; i < m_elements.size(); i++)
		m_elements[i]->render(m_spriteBatch);

	m_spriteBatch->End();
}