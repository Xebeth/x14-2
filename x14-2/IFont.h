/**************************************************************************
created		:	2013-04-12
filename	: 	IRenderable.h
author		:	Xebeth`
copyright	:	North Edge (2013)
purpose		:
**************************************************************************/
#ifndef __IFONT_H__
#define __IFONT_H__

namespace Windower
{
	class IFont
	{
	public:
		virtual ~IFont() {}

		virtual bool Initialize(LPDIRECT3DDEVICE9 pDevice_in, const TCHAR* pFaceName_in, int Size_in,
								bool Antialising_in = true, bool Bold_in = false, bool Italic_in = false) =0;
		virtual void Draw(const TCHAR* pText_in, LONG xPosition_in, LONG yPosition_in, D3DCOLOR Color_in,
						  ULONG BoxWidth_in = 0UL, ULONG BoxHeight_in = 0UL, FONTALIGNMENT Alignment_in = FA_LEFT) =0;

		virtual void OnDeviceReset() =0;
		virtual void OnDeviceLost() =0;
	};
}

#endif//__IFONT_H__