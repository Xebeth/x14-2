#ifndef __GFX_FONT_H__
#define __GFX_FONT_H__

#include "stdafx.h"

// Font alignment
enum FONTALIGNMENT { FA_LEFT, FA_CENTER, FA_RIGHT, FA_TOPRIGHT, FA_TOPLEFT, FA_BOTTOMRIGHT, FA_BOTTOMLEFT };

class Font
{
public:
	Font();
	virtual ~Font() { Release(); }

	bool Initialize(LPDIRECT3DDEVICE9 pDevice_in, const TCHAR* pFaceName_in, int Size_in,
					bool Antialising_in = true, bool Bold_in = false, bool Italic_in = false);
	void Print(const TCHAR* pText_in, int xPosition_in, int yPosition_in, DWORD Color_in, 
			   int BoxWidth_in = 0, int BoxHeight_in = 0, FONTALIGNMENT Alignment_in = FA_LEFT);
	void Release();
private:
	ID3DXSprite		*m_pTextSprite;
	LPD3DXFONT		 m_pFont;
};

#endif//__GFX_FONT_H__