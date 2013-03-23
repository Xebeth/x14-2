/**************************************************************************
	created		:	2011-10-31
	filename	: 	TestHook.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <HookEngine.h>

#include <CommandHandler.h>
#include "WindowerSettings.h"
#include "WindowerEngine.h"

#include "TestHook.h"

#include "ICoreModule.h"
#include "WindowerCore.h"
#include "TestCore.h"

extern Windower::WindowerEngine *g_pEngine;

int WINAPI sub5E3810Hook(LPVOID pThis, char *pText, int TextLen)
{
	return g_pEngine->Test().sub5E3810Hook(pThis, pText, TextLen);
}

int WINAPI sub48F5B0Hook(LPVOID pThis, char *pText)
{
	return g_pEngine->Test().sub48F5B0Hook(pThis, pText);
}

int WINAPI sub568B90Hook(LPVOID pThis, char *pText, int TextLen, signed int a4)
{
	return g_pEngine->Test().sub568B90Hook(pThis, pText, TextLen, a4);
}

bool WINAPI subA73BE0Hook(LPVOID pThis, char *pText)
{
	return g_pEngine->Test().subA73BE0Hook(pThis, pText);
}

int WINAPI sub8FFDA0Hook(LPVOID pThis, int a2, const char *a3, int a4)
{
	return g_pEngine->Test().sub8FFDA0Hook(pThis, a2, a3, a4);
}

int WINAPI sub48F4B0Hook(LPVOID pThis, char *pText, int TextLen)
{
	return g_pEngine->Test().sub48F4B0Hook(pThis, pText, TextLen);
}