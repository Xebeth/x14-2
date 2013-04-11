/**************************************************************************
	created		:	2013-04-10
	filename	: 	IWindowRenderer.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Window renderer interface
**************************************************************************/
#ifndef __I_WINDOW_RENDERER_H__
#define __I_WINDOW_RENDERER_H__

#ifndef _UIAL_SDK_INCLUDE_
	#error Please include the global header file 'UIAL.h'
#endif

#include "UiWindow.h"

namespace UIAL
{
	template <typename T, typename U> class CUiWindow;

	template <typename T = long, typename U = FRHCS> class IWindowRenderer
	{
	public:
		virtual ~IWindowRenderer() {}
	
		virtual void BeforeRender(CUiWindow<T,U> &Window_in_out) =0;
		virtual void AfterRender(CUiWindow<T,U> &Window_in_out) =0;
		virtual bool Render(CUiWindow<T,U> &Window_in_out) =0;
	};
}

#endif//__I_WINDOW_RENDERER_H__
