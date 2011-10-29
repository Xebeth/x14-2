/**************************************************************************
	created		:	2011-07-03
	filename	: 	CreateWindowExHook.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	CreateWindowEx hook definitions
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <HookEngine.h>

#include "BaseEngine.h"
#include "PluginEngine.h"
#include "BootstrapEngine.h"

#include "CreateProcessHook.h"
#include "ShellExecuteExHook.h"
#include "CreateWindowExHook.h"

#include "ICoreModule.h"
#include "WindowerCore.h"
#include "SystemCore.h"

extern Bootstrap::BootstrapEngine *g_pEngine;

/*! \brief Creates an overlapped, pop-up, or child window with an extended window style
	\param[in] dwExStyle_in : extended window style of the window being created
	\param[in] lpClassName_in : a null-terminated string or a class atom
	\param[in] lpWindowName_in : the window name
	\param[in] dwStyle_in : the style of the window being created
	\param[in] X_in : the initial horizontal position of the window
	\param[in] Y_in : the initial vertical position of the window
	\param[in] nWidth_in : the width, in device units, of the window
	\param[in] nHeight_in : the height, in device units, of the window
	\param[in] hWndParent_in : handle to the parent or owner window of the window being created
	\param[in] hMenu_in : handle to a menu, or specifies a child-window identifier, depending on the window style
	\param[in] hInstance_in : handle to the instance of the module to be associated with the window
	\param[in] lpParam_in : value to be passed to the window through the CREATESTRUCT structure
	\return a handle to the new window; NULL otherwise
*/
HWND WINAPI CreateWindowExWHook(DWORD dwExStyle_in, LPCTSTR lpClassName_in, LPCTSTR lpWindowName_in, DWORD dwStyle_in, int X_in, int Y_in,
								int nWidth_in, int nHeight_in, HWND hWndParent_in, HMENU hMenu_in, HINSTANCE hInstance_in, LPVOID lpParam_in)
{
	return g_pEngine->System().CreateWindowExWHook(dwExStyle_in, lpClassName_in, lpWindowName_in, dwStyle_in, X_in, Y_in,
												   nWidth_in, nHeight_in, hWndParent_in, hMenu_in, hInstance_in, lpParam_in);
}