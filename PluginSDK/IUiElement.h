/**************************************************************************
	created		:	2013-04-10
	filename	: 	IUiElement.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	UI Element interface
**************************************************************************/
#ifndef __I_UI_ELEMENT_H__
#define __I_UI_ELEMENT_H__

namespace Windower
{
	class IUiElement
	{
	public:
		virtual void SetSize(unsigned long Width_in, unsigned long Height_in) =0;
		virtual void Move(long XOffset_in, long YOffset_in) =0;
		virtual void SetPos(long X_in, long Y_in) =0;
		virtual void SetZOrder(long ZOrder_in) =0;

		virtual void SetBackgroundColor(unsigned long Color_in) =0;
		virtual void SetTitleColor(unsigned long Color_in) =0;
		virtual void SetTitle(const string_t &Title_in) =0;
	};
}

#endif//__I_UI_ELEMENT_H__
