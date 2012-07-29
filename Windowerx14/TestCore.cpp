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
		: WindowerCore(Engine_in_out), m_pSetJobTrampoline(NULL),
		  m_CurrentJob(JOB_NONE), m_pPlayerUI(NULL), m_pUnknownUI(NULL) {}

	/*! \brief Register the hooks for this module
		\param[in] HookManager_in : the hook manager
	*/
	void TestCore::RegisterHooks(IHookManager &HookManager_in)
	{
		SigScan::SigScan MemScan;
		DWORD_PTR dwFuncAddr;

		MemScan.Initialize(GetCurrentProcessId(), SIGSCAN_GAME_PROCESSW);

		dwFuncAddr = MemScan.Scan(TEST_SET_JOB_OPCODES_SIGNATURE,
								  TEST_SET_JOB_OPCODES_OFFSET);
		// set m_pFormatChatMessageTrampoline with the address of the original function in case of failure
		m_pSetJobTrampoline = (fnSetJob)dwFuncAddr;

		if (dwFuncAddr != NULL)
		{
			HookManager_in.RegisterHook("OnSetJob", SIGSCAN_GAME_PROCESSA, (LPVOID)dwFuncAddr,
										::SetJobHook, TEST_SET_JOB_OPCODES_HOOK_SIZE);
		}
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void TestCore::OnHookInstall(IHookManager &HookManager_in)
	{
		m_pSetJobTrampoline	= (fnSetJob)HookManager_in.GetTrampolineFunc("OnSetJob");
	}

	int TestCore::SetJob(LPVOID pThis_in_out, int *pJob_in, int Unk1_in, int Unk2_in)
	{
// 		format(m_StrDebug, _T("TestCore::SetJob>> 0x%08X 0x%08X (%hu) 0x%08X 0x%08X\n"), pThis_in_out, pJob_in, *((BYTE*)pJob_in), Unk1_in, Unk2_in);
// 		OutputDebugString(m_StrDebug.c_str());

		if (m_pUnknownUI == NULL)
			m_pUnknownUI = pThis_in_out;
		else if (m_pPlayerUI == NULL)
			m_pPlayerUI = pThis_in_out;

		if (pThis_in_out == m_pPlayerUI)
		{
			format(m_StrDebug, _T("Change job from %ld to %ld...\n"), m_CurrentJob, *((BYTE*)pJob_in));
			OutputDebugString(m_StrDebug.c_str());
			m_CurrentJob = (eJob)*((BYTE*)pJob_in);

			if (m_CurrentJob == JOB_NONE)
				m_pPlayerUI = NULL;
		}

		return m_pSetJobTrampoline(pThis_in_out, pJob_in, Unk1_in, Unk2_in);
	}
}