/**************************************************************************
	created		:	2011-05-09
	filename	: 	stdafx.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Precompiled headers support
**************************************************************************/
#ifndef __X14_AFX_H__
#define __X14_AFX_H__

// #define __USE_VLD
#include <stdhdr.h>

#define SIGSCAN_GAME_PROCESSA "ffxiv.exe"

#define __USE_DETOURS
// #define __USE_MINHOOK
// #define __USE_EASYHOOK
// #define __USE_IATHOOK

#include <SettingsManager.h>
#include <HookEngine.h>
#include <PluginSDK.h>
#include <UIAL.h>

#endif//__X14_AFX_H__