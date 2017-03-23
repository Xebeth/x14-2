/**************************************************************************
	created		:	2011-10-31
	filename	: 	TestCore.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "IGameChatPlugin.h"
#include "WindowerCore.h"
#include "TestCore.h"

#include "WindowerEngine.h"
#include "StringNode.h"

extern HINSTANCE g_hAppInstance;

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
#ifdef _M_X64
		ULONG_PTR BaseImage = reinterpret_cast<ULONG_PTR>(g_hAppInstance);
#else
		ULONG_PTR BaseImage = 0ULL;
#endif
		HookManager_in.RegisterHook("On1409E7870", SIGSCAN_GAME_PROCESSA, (LPVOID)(BaseImage + 0x001409E7870 - 0x140000000), &TestCore::Sub1409E7870Hook);
		HookManager_in.RegisterHook("On14099F950", SIGSCAN_GAME_PROCESSA, (LPVOID)(BaseImage + 0x0014099F950 - 0x140000000), &TestCore::Sub14099F950Hook);
		HookManager_in.RegisterHook("On14089CB00", SIGSCAN_GAME_PROCESSA, (LPVOID)(BaseImage + 0x0014089CB00 - 0x140000000), &TestCore::Sub14089CB00Hook);
		//HookManager_in.RegisterHook("On1409E7AA0", SIGSCAN_GAME_PROCESSA, (LPVOID)(BaseImage + 0x001409E7AA0 - 0x140000000), &TestCore::Sub1409E7AA0Hook);
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void TestCore::OnHookInstall(HookEngineLib::IHookManager &HookManager_in)
	{
		m_pSub1409E7870Trampoline = (fnSub1409E7870)HookManager_in.GetTrampolineFunc("On1409E7870");
		m_pSub14099F950Trampoline = (fnSub14099F950)HookManager_in.GetTrampolineFunc("On14099F950");
		m_pSub14089CB00Trampoline = (fnSub14089CB00)HookManager_in.GetTrampolineFunc("On14089CB00");
		m_pSub1409E7AA0Trampoline = (fnSub1409E7AA0)HookManager_in.GetTrampolineFunc("On1409E7AA0");
	}

	int TestCore::Sub1409E7870Hook(LPVOID pThis_in_out, Windower::StringNode* a2, unsigned int a3, unsigned int a4)
	{
		int Result = m_Context->m_pSub1409E7870Trampoline(pThis_in_out, a2, a3, a4);

		return Result;
	}

	int WINAPI TestCore::Sub14099F950Hook(LPVOID pThis_in_out, CHAT_MESSAGE_TYPE MessageType_in, Windower::StringNode* a2, Windower::StringNode* a3)
	{
		int Result = m_Context->m_pSub14099F950Trampoline(pThis_in_out, MessageType_in, a2, a3);

		return Result;
	}

	int TestCore::Sub14089CB00Hook(LPVOID pThis_in_out, Windower::StringNode * a2, Windower::StringNode * a3)
	{
		int Result = m_Context->m_pSub14089CB00Trampoline(pThis_in_out, a2, a3);

		return Result;
	}


	int TestCore::Sub1409E7AA0Hook(LPVOID pThis_in_out, Windower::StringNode * a2, Windower::StringNode * a3)
	{
		int Result = m_Context->m_pSub1409E7AA0Trampoline(pThis_in_out, a2, a3);

		return Result;
	}
}