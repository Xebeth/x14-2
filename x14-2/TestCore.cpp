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
//		HookManager_in.RegisterHook("OnB5FD90", SIGSCAN_GAME_PROCESSA, (LPVOID)0x00B5FD90, &TestCore::SubB5FD90Hook, 11);
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void TestCore::OnHookInstall(HookEngineLib::IHookManager &HookManager_in)
	{
		m_pSubB5FD90Trampoline = (fnSubB5FD90)HookManager_in.GetTrampolineFunc("OnB5FD90");
	}

	int TestCore::SubB5FD90Hook(LPVOID pThis, char *pDstBuf, size_t SizeInBytes)
	{
		return m_Context->m_pSubB5FD90Trampoline(pThis, pDstBuf, SizeInBytes);
	}
}