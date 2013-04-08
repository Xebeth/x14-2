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

int WINAPI Sub69C130Hook(LPVOID pThis)
{
	return g_pEngine->Test().Sub69C130Hook(pThis);
}