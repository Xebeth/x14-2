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

int WINAPI SetJobHook(LPVOID pThis_in_out, int *pJob_in, int Unk1_in, int Unk2_in)
{
	return g_pEngine->Test().SetJob(pThis_in_out, pJob_in, Unk1_in, Unk2_in);
}

int WINAPI FatSubHook(LPVOID pThis_in_out, void *a2, int a3, int a4)
{
	return g_pEngine->Test().FatSub(pThis_in_out, a2, a3, a4);
}