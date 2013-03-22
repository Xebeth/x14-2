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
		: WindowerCore(Engine_in_out, HookManager_in_out), m_IsWindowerCmd(false),
		  m_p5E0DF0Trampoline(NULL),
		  m_p490020Trampoline(NULL),
		  m_p5668C0Trampoline(NULL),
		  m_pA73810Trampoline(NULL),
		  m_p8FDF60Trampoline(NULL) 
	{
		m_Engine.RegisterModule(_T("Testing"), this);
	}

	/*! \brief Register the hooks for this module
		\param[in] HookManager_in : the hook manager
	*/
	void TestCore::RegisterHooks(IHookManager &HookManager_in)
	{
		HookManager_in.RegisterHook("On5E0DF0", SIGSCAN_GAME_PROCESSA, (LPVOID)0x005E0DF0, ::sub5E0DF0Hook, 8);
		HookManager_in.RegisterHook("On490020", SIGSCAN_GAME_PROCESSA, (LPVOID)0x00490020, ::sub490020Hook, 16);
		HookManager_in.RegisterHook("On5668C0", SIGSCAN_GAME_PROCESSA, (LPVOID)0x005668C0, ::sub5668C0Hook, 14);
		HookManager_in.RegisterHook("OnA73810", SIGSCAN_GAME_PROCESSA, (LPVOID)0x00A73810, ::subA73810Hook, 9);
		HookManager_in.RegisterHook("On8FDF60", SIGSCAN_GAME_PROCESSA, (LPVOID)0x008FDF60, ::sub8FDF60Hook, 10);
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void TestCore::OnHookInstall(IHookManager &HookManager_in)
	{
		m_p5E0DF0Trampoline = (fn5E0DF0)HookManager_in.GetTrampolineFunc("On5E0DF0");
		m_p490020Trampoline = (fn490020)HookManager_in.GetTrampolineFunc("On490020");
		m_p5668C0Trampoline = (fn5668C0)HookManager_in.GetTrampolineFunc("On5668C0");
		m_pA73810Trampoline = (fnA73810)HookManager_in.GetTrampolineFunc("OnA73810");
		m_p8FDF60Trampoline = (fn8FDF60)HookManager_in.GetTrampolineFunc("On8FDF60");
	}

	int TestCore::sub5E0DF0Hook(LPVOID pThis, char *pText, int TextLen)
	{
		if (pText != NULL && *pText == '/' && *(pText+1) == '\0')
		{
			int i = TextLen;
		}

		return m_p5E0DF0Trampoline(pThis, pText, TextLen);
	}

	int TestCore::sub490020Hook(LPVOID pThis, char *pText)
	{
		if (pText != NULL && strcmp(pText, "Aerynn Blackclaw") == 0)
		{
			int i = 0;
		}

		return m_p490020Trampoline(pThis, pText);
	}

	int TestCore::sub5668C0Hook(LPVOID pThis, char *pText, int TextLen, signed int a4)
	{
		return m_p5668C0Trampoline(pThis, pText, TextLen, a4);
	}

	bool TestCore::subA73810Hook(LPVOID pThis, char *pText)
	{
		if (m_IsWindowerCmd && pThis != NULL && strstr((char*)pThis + 16, "/echo") == (char*)pThis + 16)
			return true;

		return m_pA73810Trampoline(pThis, pText);
	}

	int TestCore::sub8FDF60Hook(LPVOID pThis, int a2, const char *a3, int a4)
	{
		m_IsWindowerCmd = (a3 && *a3 == '/' && *(a3+1) == '/');

		return m_p8FDF60Trampoline(pThis, a2, a3, a4);
	}
}