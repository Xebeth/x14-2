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
#include "StringNode.h"

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
		LONG BaseImage = (LONG)GetModuleHandle(NULL);

		// HookManager_in.RegisterHook("On59D550", SIGSCAN_GAME_PROCESSA, (LPVOID)(0x0059D550 + BaseImage - 0x00400000), &TestCore::Sub59D550Hook, 10);
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void TestCore::OnHookInstall(HookEngineLib::IHookManager &HookManager_in)
	{
		m_pSub59D550Trampoline = (fnSub59D550)HookManager_in.GetTrampolineFunc("On59D550");
	}

	int WINAPI TestCore::Sub59D550Hook(LPVOID pThis_in_out)
	{
		MEMORY_BASIC_INFORMATION memInfo;

		ZeroMemory(&memInfo, sizeof(memInfo));

		if (VirtualQuery(m_Context->m_pSub59D550Trampoline, &memInfo, sizeof(memInfo)) && (memInfo.Protect & PAGE_EXECUTE) == 0)
			VirtualProtect(m_Context->m_pSub59D550Trampoline, 9, memInfo.Protect | PAGE_EXECUTE, &memInfo.Protect);

		int Result = m_Context->m_pSub59D550Trampoline(pThis_in_out);

		return Result;
	}
}