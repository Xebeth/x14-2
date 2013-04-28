/**************************************************************************
	created		:	2013-04-06
	filename	: 	PlayerDataHook.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "WindowerEngine.h"
#include "PlayerDataHook.h"

#include "WindowerCore.h"
#include "PlayerCore.h"

extern Windower::WindowerEngine *g_pEngine;

int WINAPI CharacterMgrInitHook(LPVOID pThis)
{
	return g_pEngine->Player().CharacterMgrInitHook(pThis);
}

Windower::TargetData* WINAPI GetSelectedTargetHook(LPVOID pThis)
{
	return g_pEngine->Player().GetSelectedTargetHook(pThis);
}

int WINAPI DestroySingletonsHook(LPVOID pThis)
{
	return g_pEngine->Player().DestroySingletonsHook(pThis);
}