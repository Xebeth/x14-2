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
	TestCore::TestCore(WindowerEngine &Engine_in_out)
		: WindowerCore(Engine_in_out), m_pSetJobTrampoline(NULL) {}

	/*! \brief Register the hooks for this module
		\param[in] HookManager_in : the hook manager
	*/
	void TestCore::RegisterHooks(IHookManager &HookManager_in)
	{
		DWORD_PTR dwFuncAddr;

		dwFuncAddr = 0x570F30;
		// set m_pFormatChatMessageTrampoline with the address of the original function in case of failure
		m_pSetJobTrampoline = (fn570F30)dwFuncAddr;

		if (dwFuncAddr != NULL)
		{
			HookManager_in.RegisterHook("On570F30", SIGSCAN_GAME_PROCESSA, (LPVOID)dwFuncAddr, ::sub570F30Hook, 13);
		}
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void TestCore::OnHookInstall(IHookManager &HookManager_in)
	{
		m_pSetJobTrampoline	= (fn570F30)HookManager_in.GetTrampolineFunc("On570F30");
	}

	int TestCore::sub570F30Hook(LPVOID pThis_in_out, unsigned __int16 a2, int a3, int a4, __time64_t *ArgList)
	{
		return m_pSetJobTrampoline(pThis_in_out, a2, a3, a4, ArgList);
	}
}