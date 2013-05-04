/**************************************************************************
	created		:	2011-03-06
	filename	: 	stdafx.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Precompiled header
**************************************************************************/
#ifndef __VISUAL_MANAGER_AFX_H__
#define __VISUAL_MANAGER_AFX_H__

// #define __USE_VLD
#include <stdhdr.h>
#include <map>

#include <afxext.h>				// MFC extensions
#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

#ifdef _LIB
	#define APPVIMGR_API
#else
	#define APPVIMGR_API __declspec(dllexport)
#endif

#ifdef _UNICODE
	#if defined _M_IX86
		#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
	#elif defined _M_IA64
		#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
	#elif defined _M_X64
		#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
	#else
		#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
	#endif
#endif

#endif//__VISUAL_MANAGER_AFX_H__