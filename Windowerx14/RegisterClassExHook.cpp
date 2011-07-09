/**************************************************************************
	created		:	2011-06-03
	filename	: 	RegisterClassExHook.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	RegisterClassEx hook definitions
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

#include "ICoreModule.h"
#include "WindowerCore.h"
#include "SystemCore.h"

extern Windower::WindowerEngine *g_pEngine;

ATOM WINAPI RegisterClassExWHook(const WNDCLASSEXW *pWndClass)
{
	return g_pEngine->System().RegisterClassExWHook(const_cast<WNDCLASSEXW*>(pWndClass));
}