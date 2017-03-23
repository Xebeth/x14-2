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

#ifdef _M_X64
	#define SIGSCAN_GAME_PROCESSA "ffxiv_dx11.exe"
#else
	#define SIGSCAN_GAME_PROCESSA "ffxiv.exe"
#endif

#define __USE_DETOURS
// #define __USE_MINHOOK
// #define __USE_EASYHOOK
// #define __USE_IATHOOK

#include <SettingsManager.h>
#include <HookEngine.h>
#include <PluginSDK.h>

#define DLL_QUERY_HMODULE		6

#define DEREF( name )*(UINT_PTR *)(name)
#define DEREF_64( name )*(DWORD64 *)(name)
#define DEREF_32( name )*(DWORD *)(name)
#define DEREF_16( name )*(WORD *)(name)
#define DEREF_8( name )*(BYTE *)(name)

typedef ULONG_PTR(WINAPI * REFLECTIVELOADER)(VOID);
typedef BOOL(WINAPI * DLLMAIN)(HINSTANCE, DWORD, LPVOID);

#define DLLEXPORT   __declspec( dllexport ) 

#endif//__X14_AFX_H__