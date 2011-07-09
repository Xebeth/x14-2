#include "stdafx.h"
#include <d3dx9.h>
#include <d3d9.h>
#include <vector>

#include "Font.h"

/*! \brief Default constructor. */
Font::Font()
{
	m_pTextSprite = NULL;
	m_pFont = NULL; 
}

bool Font::Initialize(LPDIRECT3DDEVICE9 pDevice_in, const TCHAR* pFaceName, int Size_in, bool Antialising_in, bool Bold_in, bool Italic_in)
{
	Release();

	D3DXCreateSprite(pDevice_in, &m_pTextSprite);

	return D3DXCreateFont(pDevice_in, -Size_in, 0, Bold_in ? FW_BOLD : FW_NORMAL,
						  1, Italic_in, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
						  DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
						  pFaceName, &m_pFont) == D3D_OK;
}

void Font::Print(const TCHAR* pText_in, int xPosition_in, int yPosition_in, DWORD Color_in,
				 int BoxWidth_in, int BoxHeight_in, FONTALIGNMENT Alignment_in)
{
	if (m_pFont == NULL || m_pTextSprite == NULL || pText_in == NULL)
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
	RECT rect = { xPosition_in, yPosition_in, xPosition_in + BoxWidth_in, yPosition_in + BoxHeight_in };

	m_pTextSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
	m_pFont->DrawText(m_pTextSprite, pText_in, -1, &rect, format, Color_in);
	m_pTextSprite->End();
}

void Font::Release()
{
	m_pFont = NULL;
	m_pTextSprite = NULL;
}