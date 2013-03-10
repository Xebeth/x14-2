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

DWORD g_5AEB30JumpAddr = NULL;

namespace Windower
{
	/*! \brief TestCore constructor
		\param[in,out] pEngine : the windower engine
	*/
	TestCore::TestCore(WindowerEngine &Engine_in_out)
		: WindowerCore(Engine_in_out), m_p98B710Trampoline(NULL) {}

	/*! \brief Register the hooks for this module
		\param[in] HookManager_in : the hook manager
	*/
	void TestCore::RegisterHooks(IHookManager &HookManager_in)
	{
		HookManager_in.RegisterHook("On98B710", SIGSCAN_GAME_PROCESSA, (LPVOID)0x0098B710, ::sub98B710Hook, 11);
		HookManager_in.RegisterHook("On5AEB30", SIGSCAN_GAME_PROCESSA, (LPVOID)0x005AEB30, ::sub5AEB30NakedHook);
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void TestCore::OnHookInstall(IHookManager &HookManager_in)
	{
		m_p98B710Trampoline = (fn98B710)HookManager_in.GetTrampolineFunc("On98B710");
		g_5AEB30JumpAddr = (DWORD)HookManager_in.GetTrampolineFunc("On5AEB30");
	}

	char TestCore::sub98B710Hook(LPVOID pThis, int a2, int a3, int a4, int a5)
	{
		return m_p98B710Trampoline(pThis, a2, a3, a4, a5);
	}

	void TestCore::sub5AEB30Hook(int a1, int a2)
	{
		
	}
}