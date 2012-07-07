/**************************************************************************
	created		:	2011-05-09
	filename	: 	stdafx.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Precompiled headers support
**************************************************************************/
#include "stdafx.h"
#include <detours.h>

#ifdef _DEBUG
	#ifdef _M_X64
		#pragma comment(lib, "PluginFramework.x64d.lib")
		#pragma comment(lib, "SettingsManager.x64d.lib")
		#pragma comment(lib, "WindowerPlugin.x64d.lib")
		#pragma comment(lib, "HookManager.x64d.lib")
		#pragma comment (lib, "detours_d.lib")
	#else
		#pragma comment(lib, "PluginFramework.x86d.lib")
		#pragma comment(lib, "SettingsManager.x86d.lib")
		#pragma comment(lib, "WindowerPlugin.x86d.lib")
		#pragma comment(lib, "HookManager.x86d.lib")
		#pragma comment (lib, "detours_d.lib")
		#pragma comment (lib, "IATPatcher.x86d.lib")
	#endif
#else
	#ifdef _M_X64
		#pragma comment(lib, "PluginFramework.x64.lib")
		#pragma comment(lib, "SettingsManager.x64.lib")
		#pragma comment(lib, "WindowerPlugin.x64.lib")
		#pragma comment(lib, "HookManager.x64.lib")
		#pragma comment (lib, "detours.lib")
	#else
		#pragma comment(lib, "PluginFramework.x86.lib")
		#pragma comment(lib, "SettingsManager.x86.lib")
		#pragma comment(lib, "WindowerPlugin.x86.lib")
		#pragma comment(lib, "HookManager.x86.lib")
		#pragma comment (lib, "detours.lib")
		#pragma comment (lib, "IATPatcher.x86.lib")
	#endif
#endif

// this function is exported on ordinal 1 so that the DLL can be loaded with DetourCreateProcessWithDll
extern "C" __declspec(dllexport) void DetourAndErrorCheck(PVOID* pFunctionToDetour, PVOID pReplacement, const char* functionName)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourAttach(pFunctionToDetour, pReplacement);

	DetourTransactionCommit();
}