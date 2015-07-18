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
		ULONG_PTR BaseImage = 0ULL;

		HookManager_in.RegisterHook("On140966890", SIGSCAN_GAME_PROCESSA, (LPVOID)(BaseImage + 0x00140966890 - 0x140000000), &TestCore::Sub140966890Hook);
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void TestCore::OnHookInstall(HookEngineLib::IHookManager &HookManager_in)
	{
		m_pSub140966890Trampoline = (fnSub140966890)HookManager_in.GetTrampolineFunc("On140966890");
	}

	int WINAPI TestCore::Sub140966890Hook(LPVOID pThis_in_out, Windower::StringNode a2, __int64 a3)
	{
		int Result = m_Context->m_pSub140966890Trampoline(pThis_in_out, a2, a3);

		return Result;
	}
}