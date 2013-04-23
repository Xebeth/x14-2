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

int WINAPI Sub5D1C70Hook(LPVOID pThis, DWORD a2, char *a3, int a4, int a5)
{
	return g_pEngine->Test().Sub5D1C70Hook(pThis, a2, a3, a4, a5);
}

int WINAPI Sub5E9E30Hook(LPVOID pThis, StringNode * a2)
{
	return g_pEngine->Test().Sub5E9E30Hook(pThis, a2);
}