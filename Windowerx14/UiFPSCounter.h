/**************************************************************************
	created		:	2013-04-12
	filename	: 	UiFPSCounter.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __UI_FPS_COUNTER_H__
#define __UI_FPS_COUNTER_H__

struct IDirect3DDevice9;
class Timer;

namespace Windower
{
	class UiFPSCounter : public UiTextLabel
	{
	public:
		UiFPSCounter(unsigned long ID_in, IDirect3DDevice9 *pDevice_in, const string_t& Name_in,
					  long X_in, long Y_in, long W_in, long H_in, const string_t &FontName_in,
					  unsigned short FontSize_in, bool bBold_in, bool bItalic_in, unsigned long ARGB_in,
					  UIAL::IWindowRenderer<> *pRenderer_in, bool Visibile_in = true);
		~UiFPSCounter();

		void Update();

	protected:
		//! render timer
		Timer *m_pRenderTimer;
	};
}

#endif//__UIFPSCOUNTER_H__
