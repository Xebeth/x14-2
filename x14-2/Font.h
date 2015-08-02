/**************************************************************************
	created		:	2011-10-30
	filename	: 	Font.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Font class
**************************************************************************/
#ifndef __GFX_FONT_H__
#define __GFX_FONT_H__

//! Font alignment
enum FONTALIGNMENT
{
	FA_LEFT = 0,		// left
	FA_CENTER,			// center
	FA_RIGHT,			// right
	FA_TOPRIGHT,		// top-right
	FA_TOPLEFT,			// top-left
	FA_BOTTOMRIGHT,		// bottom-right
	FA_BOTTOMLEFT		// bottom-left
};

//! \brief Font class
class Font
{
public:
	Font();
	virtual ~Font();

	bool Initialize(LPDIRECT3DDEVICE9 pDevice_in, const TCHAR* pFaceName_in, int Size_in,
					bool Antialising_in = true, bool Bold_in = false, bool Italic_in = false);
	void Print(const TCHAR* pText_in, LONG xPosition_in, LONG yPosition_in, D3DCOLOR Color_in,
			   LONG BoxWidth_in = 0L, LONG BoxHeight_in = 0L, FONTALIGNMENT Alignment_in = FA_LEFT);

	void Release();
	void OnLostDevice();
	void OnDeviceReset();

	ID3DXSprite* GetSprite() const;
private:
	//! the sprite used to draw text
	ID3DXSprite		*m_pTextSprite;
	//! the font
	LPD3DXFONT		 m_pFont;
};

#endif//__GFX_FONT_H__