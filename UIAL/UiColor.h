/**************************************************************************
	created		:	2013-04-10
	filename	: 	UiColor.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	UI Color
**************************************************************************/
#ifndef __UI_COLOR_H__
#define __UI_COLOR_H__

#ifndef _UIAL_SDK_INCLUDE_
	#error Please include the global header file 'UIAL.h'
#endif

namespace UIAL
{
	#define ALPHA_SHIFT		0x00U
	#define GREEN_SHIFT		0x10U
	#define BLUE_SHIFT		0x18U
	#define RED_SHIFT		0x08U

	class CUiColor
	{
	public:
		CUiColor(unsigned char Red_in, unsigned char Green_in,
				 unsigned char Blue_in, unsigned char Alpha_in = 255U);
		CUiColor(unsigned long ARGB_in);
		CUiColor();
		
		unsigned char GetAlpha() const;
		unsigned char GetGreen() const;
		unsigned char GetBlue() const;
		unsigned char GetRed() const;

		void SetColor(unsigned char Red_in, unsigned char Green_in,
					  unsigned char Blue_in, unsigned char Alpha_in = 255U);
		void SetColor(unsigned long ARGB_in);

		void SetAlpha(unsigned char Alpha_in);
		void SetGreen(unsigned char Green_in);
		void SetBlue(unsigned char Blue_in);
		void SetRed(unsigned char Red_in);

	private:
		void SetComponent(unsigned char Value_in, unsigned char Shift_in);
		unsigned char GetComponent(unsigned char Shift_in) const;		

		//! 32-bit color packed ARGB values (AARRGGBB)
		unsigned long m_ARGB;
	};
}

#endif//__UI_COLOR_H__
