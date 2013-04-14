/**************************************************************************
	created		:	2011-10-31
	filename	: 	TestHook.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "WindowerEngine.h"
#include "TestHook.h"

#include "WindowerCore.h"
#include "TestCore.h"

extern Windower::WindowerEngine *g_pEngine;

int WINAPI Sub4357A0Hook(LPVOID pThis, DWORD a2, char *a3, int a4, int a5)
{
// 	OutputDebugStringA(a3);
// 	OutputDebugStringA("\n");

	if (strstr(a3, "Draw") == NULL && strstr(a3, "draw") == NULL && strstr(a3, "Render") == NULL && strstr(a3, "Display") == NULL)
	{
		OutputDebugStringA(a3);
		OutputDebugStringA("\n");
	}

	return g_pEngine->Test().Sub4357A0Hook(pThis, a2, a3, a4, a5);
}