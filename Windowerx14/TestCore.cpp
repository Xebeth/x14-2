/**************************************************************************
	created		:	2011-10-31
	filename	: 	TestCore.cpp
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

#include "RegisterClassExHook.h"
#include "CreateWindowExHook.h"
#include "WndProcHook.h"
#include "TestHook.h"

#include "WindowerSettings.h"

#include "ICoreModule.h"
#include "WindowerCore.h"
#include "TestCore.h"

#include <SigScan.h>

namespace Windower
{
	/*! \brief TestCore constructor
		\param[in,out] pEngine : the windower engine
	*/
	TestCore::TestCore(WindowerEngine &Engine_in_out, HookEngine &HookManager_in_out)
		: WindowerCore(_T("Testing"), Engine_in_out, HookManager_in_out),
		  m_IsWindowerCmd(false),
		  m_p5E3810Trampoline(NULL),
		  m_p48F5B0Trampoline(NULL),
		  m_p568B90Trampoline(NULL),
		  m_pA73BE0Trampoline(NULL),
		  m_p8FFDA0Trampoline(NULL),
		  m_p48F4B0Trampoline(NULL) {}

	/*! \brief Register the hooks for this module
		\param[in] HookManager_in : the hook manager
	*/
	void TestCore::RegisterHooks(IHookManager &HookManager_in)
	{
 		HookManager_in.RegisterHook("On5E3810", SIGSCAN_GAME_PROCESSA, (LPVOID)0x005E3810, ::sub5E3810Hook, 8);
 		HookManager_in.RegisterHook("On48F5B0", SIGSCAN_GAME_PROCESSA, (LPVOID)0x0048F5B0, ::sub48F5B0Hook, 16);
 		HookManager_in.RegisterHook("On568B90", SIGSCAN_GAME_PROCESSA, (LPVOID)0x00568B90, ::sub568B90Hook, 14);
 		HookManager_in.RegisterHook("OnA73BE0", SIGSCAN_GAME_PROCESSA, (LPVOID)0x00A73BE0, ::subA73BE0Hook, 9);
 		HookManager_in.RegisterHook("On8FFDA0", SIGSCAN_GAME_PROCESSA, (LPVOID)0x008FFDA0, ::sub8FFDA0Hook, 11);
 		HookManager_in.RegisterHook("On48F4B0", SIGSCAN_GAME_PROCESSA, (LPVOID)0x0048F4B0, ::sub48F4B0Hook, 8);
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void TestCore::OnHookInstall(IHookManager &HookManager_in)
	{
		m_p5E3810Trampoline = (fn5E3810)HookManager_in.GetTrampolineFunc("On5E3810");
		m_p48F5B0Trampoline = (fn48F5B0)HookManager_in.GetTrampolineFunc("On48F5B0");
		m_p568B90Trampoline = (fn568B90)HookManager_in.GetTrampolineFunc("On568B90");
		m_pA73BE0Trampoline = (fnA73BE0)HookManager_in.GetTrampolineFunc("OnA73BE0");
		m_p8FFDA0Trampoline = (fn8FFDA0)HookManager_in.GetTrampolineFunc("On8FFDA0");
		m_p48F4B0Trampoline = (fn48F4B0)HookManager_in.GetTrampolineFunc("On48F4B0");
	}

	int TestCore::sub5E3810Hook(LPVOID pThis, char *pText, int TextLen)
	{
		if (pText != NULL && ((*pText == '/' && *(pText+1) == '\0') /*|| strstr(pText, "Version: ") != NULL || strstr(pText, "2013.") == pText*/))
		{
			int i = TextLen;
		}

		return m_p5E3810Trampoline(pThis, pText, TextLen);
	}

	int TestCore::sub48F5B0Hook(LPVOID pThis, char *pText)
	{
		if (pText != NULL && strcmp(pText, "Aerynn Blackclaw") == 0)
		{
			int i = 0;
		}

		return m_p48F5B0Trampoline(pThis, pText);
	}

	int TestCore::sub568B90Hook(LPVOID pThis, char *pText, int TextLen, signed int a4)
	{
		return m_p568B90Trampoline(pThis, pText, TextLen, a4);
	}

	bool TestCore::subA73BE0Hook(LPVOID pThis, char *pText)
	{
		if (m_IsWindowerCmd && pThis != NULL && strstr((char*)pThis + 16, "/echo") == (char*)pThis + 16)
			return true;

		return m_pA73BE0Trampoline(pThis, pText);
	}

	int TestCore::sub8FFDA0Hook(LPVOID pThis, int a2, const char *a3, int a4)
	{
		m_IsWindowerCmd = (a3 && *a3 == '/' && *(a3+1) == '/');

		return m_p8FFDA0Trampoline(pThis, a2, a3, a4);
	}

	int TestCore::sub48F4B0Hook(LPVOID pThis, char *pText, int TextLen)
	{
		return m_p48F4B0Trampoline(pThis, pText, TextLen);
	}
}