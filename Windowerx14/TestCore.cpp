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

#include "WindowerEngine.h"

namespace Windower
{
	/*! \brief TestCore constructor
		\param[in,out] pEngine : the windower engine
	*/
	TestCore::TestCore(WindowerEngine &Engine_in_out, HookEngine &HookManager_in_out)
		: WindowerCore(_T("Testing"), Engine_in_out, HookManager_in_out),
		  m_pSub4357A0Trampoline(NULL) {}

	/*! \brief Register the hooks for this module
		\param[in] HookManager_in : the hook manager
	*/
	void TestCore::RegisterHooks(HookEngineLib::IHookManager &HookManager_in)
	{
 	  	//HookManager_in.RegisterHook("On4357A0", SIGSCAN_GAME_PROCESSA, (LPVOID)0x004357A0, ::Sub4357A0Hook, 13);
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void TestCore::OnHookInstall(HookEngineLib::IHookManager &HookManager_in)
	{
		m_pSub4357A0Trampoline = (fnSub4357A0)HookManager_in.GetTrampolineFunc("On4357A0");
	}

	int TestCore::Sub4357A0Hook(LPVOID pThis, DWORD a2, char * a3, int a4, int a5)
	{
		return m_pSub4357A0Trampoline(pThis, a2, a3, a4, a5);
	}
}