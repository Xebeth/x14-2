/**************************************************************************
	created		:	2011-06-04
	filename	: 	WndProcHook.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	WndProc hook definitions
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

LRESULT WINAPI WndProcHook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return g_pEngine->System().WndProcHook(hWnd, uMsg, wParam, lParam);
}