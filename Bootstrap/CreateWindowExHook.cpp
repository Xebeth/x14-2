/**************************************************************************
	created		:	2011-07-03
	filename	: 	CreateWindowExHook.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	CreateWindowEx hook definitions
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <NonCopyable.h>
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

HWND WINAPI CreateWindowExWHook(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int X, int Y,
								int nWidth, int nHeight,HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	return g_pEngine->System().CreateWindowExWHook(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y,
												   nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}