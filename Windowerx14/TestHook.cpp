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

int WINAPI sub5E0DF0Hook(LPVOID pThis, char *pText, int TextLen)
{
	return g_pEngine->Test().sub5E0DF0Hook(pThis, pText, TextLen);
}

int WINAPI sub490020Hook(LPVOID pThis, char *pText)
{
	return g_pEngine->Test().sub490020Hook(pThis, pText);
}

int WINAPI sub5668C0Hook(LPVOID pThis, char *pText, int TextLen, signed int a4)
{
	return g_pEngine->Test().sub5668C0Hook(pThis, pText, TextLen, a4);
}

bool WINAPI subA73810Hook(LPVOID pThis, char *pText)
{
	return g_pEngine->Test().subA73810Hook(pThis, pText);
}

int WINAPI sub8FDF60Hook(LPVOID pThis, int a2, const char *a3, int a4)
{
	return g_pEngine->Test().sub8FDF60Hook(pThis, a2, a3, a4);
}