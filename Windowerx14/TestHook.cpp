/**************************************************************************
	created		:	2011-10-31
	filename	: 	TestHook.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "WindowerSettings.h"
#include "WindowerEngine.h"

#include "TestHook.h"

#include "WindowerCore.h"
#include "TestCore.h"

extern Windower::WindowerEngine *g_pEngine;

int WINAPI Sub868970Hook(LPVOID pThis)
{
	return g_pEngine->Test().Sub868970Hook(pThis);
}

int WINAPI Sub83FD00Hook(LPVOID pThis)
{
	return g_pEngine->Test().Sub83FD00Hook(pThis);
}