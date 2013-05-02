/**************************************************************************
	created		:	2011-10-31
	filename	: 	TestCore.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "WindowerCore.h"
#include "TestCore.h"

#include "WindowerEngine.h"

namespace Windower
{
	WindowerCore::CallingContext<TestCore> TestCore::m_Context;

	/*! \brief TestCore constructor
		\param[in,out] pEngine : the windower engine
	*/
	TestCore::TestCore() : WindowerCore(_T(TESTING_MODULE))
	{
		// set the calling context for the hooks
		m_Context.Set(this);
	}

	/*! \brief Register the hooks for this module
		\param[in] HookManager_in : the hook manager
	*/
	void TestCore::RegisterHooks(HookEngineLib::IHookManager &HookManager_in)
	{
//		HookManager_in.RegisterHook("On5D1C70", SIGSCAN_GAME_PROCESSA, (LPVOID)0x005D1C70, ::Sub5D1C70Hook, 9);
// 		HookManager_in.RegisterHook("On5E9E30", SIGSCAN_GAME_PROCESSA, (LPVOID)0x005E9E30, ::Sub5E9E30Hook, 8);
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void TestCore::OnHookInstall(HookEngineLib::IHookManager &HookManager_in)
	{
		m_pSub5D1C70Trampoline = (fnSub5D1C70)HookManager_in.GetTrampolineFunc("On5D1C70");
		m_pSub5E9E30Trampoline = (fnSub5E9E30)HookManager_in.GetTrampolineFunc("On5E9E30");
	}

	int TestCore::Sub5D1C70Hook(LPVOID pThis, DWORD a2, char * a3, int a4, int a5)//a5 => StringNode
	{
		return m_Context->m_pSub5D1C70Trampoline(pThis, a2, a3, a4, a5);
	}

	int TestCore::Sub5E9E30Hook(LPVOID pThis, StringNode *a2)
	{
		return m_Context->m_pSub5E9E30Trampoline(pThis, a2);
	}
}