/**************************************************************************
	created		:	2011-10-30
	filename	: 	Font.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Font class
**************************************************************************/
#include "stdafx.h"
#include <d3d9.h>
#include <d3dx9.h>

#include "Font.h"

/*! \brief Font default constructor */
Font::Font()
{
	m_pTextSprite = nullptr;
	m_pFont = nullptr; 
}

//! Font destructor
Font::~Font()
{
	Release();
}

/*! \brief Initializes the font
	\param[in] pDevice_in : the Direct3D device
	\param[in] pFaceName_in : the name of the font face
	\param[in] Size_in : the size of the font
	\param[in] Antialising_in : flag specifying if the font is anti-aliased
	\param[in] Bold_in : flag specifying if the font is bold
	\param[in] Italic_in : flag specifying if the font is italic
	\return true if the font was created successfully; false otherwise
*/
bool Font::Initialize(LPDIRECT3DDEVICE9 pDevice_in, const TCHAR* pFaceName_in, int Size_in, bool Antialising_in, bool Bold_in, bool Italic_in)
{
	HRESULT Result;

	Release();

	// create the sprite
	Result = D3DXCreateSprite(pDevice_in, &m_pTextSprite);

	if (SUCCEEDED(Result))
	{
		Result = D3DXCreateFont(pDevice_in, -Size_in, 0, Bold_in ? FW_BOLD : FW_NORMAL,
								1, Italic_in, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
								DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
								pFaceName_in, &m_pFont);
	}

	return (Result == D3D_OK);
}

/*! \brief Displays text at the specified position
	\param[in] pText_in : the text to be displayed
	\param[in] xPosition_in : the x-coordinate of the text
	\param[in] yPosition_in : the y-coordinate of the text
	\param[in] Color_in : the color of the text
	\param[in] BoxWidth_in : the width of the box used for alignment
	\param[in] BoxHeight_in : the height of the box used for alignment 
	\param[in] Alignment_in : the alignment of the text
*/
void Font::Print(const TCHAR* pText_in, LONG xPosition_in, LONG yPosition_in, D3DCOLOR Color_in,
				 LONG BoxWidth_in, LONG BoxHeight_in, FONTALIGNMENT Alignment_in)
{
	if (m_pFont == nullptr || m_pTextSprite == nullptr || pText_in == nullptr)
		return;

	DWORD format = DT_EXPANDTABS;

	if (BoxWidth_in == 0)
	{
		format |= DT_NOCLIP;
	}
	else
	{
		format |= DT_WORDBREAK;
		switch (Alignment_in)
		{
		case FA_LEFT:
			format |= DT_LEFT;
			break;
		case FA_CENTER:
			format |= DT_CENTER;
			break;
		case FA_RIGHT:
			format |= DT_RIGHT;
			break;
		case FA_TOPRIGHT:
			format |= DT_RIGHT | DT_TOP;
			break;
		case FA_BOTTOMRIGHT:
			format |= DT_RIGHT | DT_BOTTOM;
			break;
		case FA_TOPLEFT:
			format |= DT_LEFT | DT_TOP;
			break;
		case FA_BOTTOMLEFT:
			format |= DT_LEFT | DT_BOTTOM;
			break;
		}
		if (BoxHeight_in == 0)
		{
			// A width is specified, but not a height.
			// Make it seem like height is infinite
			BoxHeight_in = 2000;
		}
	}

	RECT SpriteRect = { xPosition_in, yPosition_in, xPosition_in + BoxWidth_in, yPosition_in + BoxHeight_in };

	m_pTextSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
	m_pFont->DrawText(m_pTextSprite, pText_in, -1, &SpriteRect, format, Color_in);
	m_pTextSprite->End();
}

//! \brief Destroys the font data
void Font::Release()
{
	m_pTextSprite = nullptr;
	m_pFont = nullptr;
}

ID3DXSprite* Font::GetSprite() const
{
	return m_pTextSprite;
}

void Font::OnDeviceReset()
{
	if (m_pTextSprite != nullptr)
		m_pTextSprite->OnResetDevice();

	if (m_pFont != nullptr)
		m_pFont->OnResetDevice();
}

void Font::OnLostDevice()
{
	if (m_pTextSprite != nullptr)
		m_pTextSprite->OnLostDevice();

	if (m_pFont != nullptr)
		m_pFont->OnLostDevice();
}