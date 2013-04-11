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
		  m_pSub69C130Trampoline(NULL) {}

	/*! \brief Register the hooks for this module
		\param[in] HookManager_in : the hook manager
	*/
	void TestCore::RegisterHooks(HookEngineLib::IHookManager &HookManager_in)
	{
	  	//HookManager_in.RegisterHook("On69C130", SIGSCAN_GAME_PROCESSA, (LPVOID)0x0069C130, ::Sub69C130Hook, 11);
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void TestCore::OnHookInstall(HookEngineLib::IHookManager &HookManager_in)
	{
		//m_pSub69C130Trampoline = (fnSub69C130)HookManager_in.GetTrampolineFunc("On69C130");
	}

	int TestCore::Sub69C130Hook(LPVOID pThis)
	{
		int Result = m_pSub69C130Trampoline(pThis);

		return Result;
	}
}