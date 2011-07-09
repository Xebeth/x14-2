/**************************************************************************
	created		:	2011-06-03
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
#include "WindowerEngine.h"

#include "RegisterClassExHook.h"
#include "CreateWindowExHook.h"
#include "WndProcHook.h"

#include "ICoreModule.h"
#include "WindowerCore.h"
#include "SystemCore.h"

extern Windower::WindowerEngine *g_pEngine;

HWND WINAPI CreateWindowExAHook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y,
								int nWidth, int nHeight,HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	return g_pEngine->System().CreateWindowExAHook(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y,
												   nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}