/**************************************************************************
	created		:	2011-10-31
	filename	: 	TestCore.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "TestHook.h"

#include "WindowerCore.h"
#include "TestCore.h"

#include "WindowerSettings.h"
#include "WindowerEngine.h"

namespace Windower
{
	/*! \brief TestCore constructor
		\param[in,out] pEngine : the windower engine
	*/
	TestCore::TestCore(WindowerEngine &Engine_in_out, HookEngine &HookManager_in_out)
		: WindowerCore(_T("Testing"), Engine_in_out, HookManager_in_out),
		  m_pSub868970Trampoline(NULL),
		  m_pSub83FD00Trampoline(NULL) {}

	/*! \brief Register the hooks for this module
		\param[in] HookManager_in : the hook manager
	*/
	void TestCore::RegisterHooks(HookEngineLib::IHookManager &HookManager_in)
	{
// 	  	HookManager_in.RegisterHook("On868970", SIGSCAN_GAME_PROCESSA, (LPVOID)0x00868970, ::Sub868970Hook, 11);
// 		HookManager_in.RegisterHook("On83FD00", SIGSCAN_GAME_PROCESSA, (LPVOID)0x0083FD00, ::Sub83FD00Hook, 14);
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void TestCore::OnHookInstall(HookEngineLib::IHookManager &HookManager_in)
	{
		m_pSub868970Trampoline = (fnSub868970)HookManager_in.GetTrampolineFunc("On868970");
		m_pSub83FD00Trampoline = (fnSub83FD00)HookManager_in.GetTrampolineFunc("On83FD00");
	}

	int TestCore::Sub868970Hook(LPVOID pThis)
	{
		int Result = m_pSub868970Trampoline(pThis);

		return Result;
	}

	int TestCore::Sub83FD00Hook(LPVOID pThis)
	{
		int Result = m_pSub83FD00Trampoline(pThis);

		return Result;
	}
}