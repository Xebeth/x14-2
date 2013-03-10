/**************************************************************************
	created		:	2011-05-09
	filename	: 	stdafx.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Precompiled headers support
**************************************************************************/
#include "stdafx.h"

#ifdef _DEBUG
	#ifdef _M_X64
		#pragma comment(lib, "Sigscan.x64d.lib")
		#pragma comment(lib, "HookManager.x64d.lib")
		#pragma comment(lib, "PluginFramework.x64d.lib")
		#pragma comment(lib, "SettingsManager.x64d.lib")
		#pragma comment(lib, "WindowerPlugin.x64d.lib")
	#else
		#pragma comment(lib, "Sigscan.x86d.lib")
		#pragma comment(lib, "HookManager.x86d.lib")
		#pragma comment(lib, "PluginFramework.x86d.lib")
		#pragma comment(lib, "SettingsManager.x86d.lib")
		#pragma comment(lib, "WindowerPlugin.x86d.lib")
	#endif
#else
	#ifdef _M_X64
		#pragma comment(lib, "Sigscan.x64.lib")
		#pragma comment(lib, "HookManager.x64.lib")
		#pragma comment(lib, "PluginFramework.x64.lib")
		#pragma comment(lib, "SettingsManager.x64.lib")
		#pragma comment(lib, "WindowerPlugin.x64.lib")
	#else
		#pragma comment(lib, "Sigscan.x86.lib")
		#pragma comment(lib, "HookManager.x86.lib")
		#pragma comment(lib, "PluginFramework.x86.lib")
		#pragma comment(lib, "SettingsManager.x86.lib")
		#pragma comment(lib, "WindowerPlugin.x86.lib")
	#endif
#endif

#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")

// #pragma comment(lib, "dxguid.lib")
// #pragma comment(lib, "dinput8.lib")

#ifdef __USE_DETOURS
	#include <detours.h>

	#pragma comment(lib, "detours.lib")
	// this function is exported on ordinal 1 so that the DLL can be loaded with DetourCreateProcessWithDll
	extern "C" __declspec(dllexport) void DetourAndErrorCheck(PVOID* pFunctionToDetour, PVOID pReplacement, const char* functionName)
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourAttach(pFunctionToDetour, pReplacement);

		DetourTransactionCommit();
	}
#endif // _DEBUG