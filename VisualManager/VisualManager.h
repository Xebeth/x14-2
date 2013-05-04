/**************************************************************************
	created		:	2013-05-03
	filename	: 	VisualManager.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __VISUAL_MANAGER_H__
#define __VISUAL_MANAGER_H__

#define APPVIMGR_API // __declspec(dllimport)

#include <map>
#include <afxext.h>				// MFC extensions
#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

#define _VISUAL_MANAGER_SDK_INCLUDE_

#include <VisualMgr.h>

#include <TabPage.h>
#include <TabHeader.h>
#include <VisualTab.h>
#include <VisualDialog.h>
#include <VisualTabCtrl.h>
#include <VisualTabDialog.h>
#include <ImageListButton.h>

#ifdef _DEBUG
	#pragma comment(lib, "VisualManager.x86d.lib")
#else
	#pragma comment(lib, "VisualManager.x86.lib")
#endif // _DEBUG

// macro used to easily change the base class of dialogs
#define DialogBaseClass CDialogEx // CDialog
// macro used to easily change the base class of frames
#define FrameBaseClass CFrameWndEx // CFrameWnd
// macro used to easily change the base class of the main frame
#define MainFrameBaseClass CMDIFrameWndEx
// macro used to easily change the base class of buttons
#define ButtonBaseClass CButton // CButton

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

#endif//__VISUAL_MANAGER_H__
