/**************************************************************************
	created		:	2011-06-15
	filename	: 	IATHookManager.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Hook manager using IAT Patcher
**************************************************************************/
#include "stdafx.h"
#ifdef __USE_IATHOOK

#include <ImportTablePatcher.h>

#include "HookManager.h"
#include "IATHookManager.h"

#include "Hook.h"

#ifdef _DEBUG
	#ifdef _M_X64
		#pragma comment(lib, "IATPatcher.x64d.lib")
	#else
		#pragma comment(lib, "IATPatcher.x86d.lib")
	#endif
#else
	#ifdef _M_X64
		#pragma comment(lib, "IATPatcher.x64.lib")
	#else
		#pragma comment(lib, "IATPatcher.x86.lib")
	#endif
#endif // _DEBUG

namespace HookEngineLib
{
	/*! \brief Initializes the hooking process
		\return true if the initialization succeeded; false otherwise
	*/
	bool IATHookManager::Initialize()
	{
		m_hProcess = GetModuleHandle(NULL);
		m_bInit = (m_hProcess != NULL);

		return m_bInit;
	}

	/*! \brief Terminates the hooking process
		\return true if the termination succeeded; false otherwise
	*/
	void IATHookManager::Shutdown()
	{
		IHookManager::Shutdown();
		m_hProcess = NULL;
	}

	/*! \brief Creates a hook by patching the original function
		\param[in] pHook_in : the hook to create
		\return true if the hook was created; false otherwise
	*/
	bool IATHookManager::CreateHook(Hook *pHook_in_out)
	{
		// use Detours to install the hook
		return IATPatcher::PatchIAT(m_hProcess, pHook_in_out->m_sModuleName.c_str(),
									pHook_in_out->m_sFuncName.c_str(),
									&pHook_in_out->m_pTrampolineFunc,
									pHook_in_out->m_pHookFunc);
	}

	/*! \brief Destroys a hook by restoring the original function
		\param[in] pHook_in : the hook to destroy
		\return true if the hook was destroyed; false otherwise
	*/
	bool IATHookManager::DestroyHook(const Hook *pHook_in)
	{
		// use Detours to remove the hook
		return IATPatcher::RestoreIAT(m_hProcess, pHook_in->m_sModuleName.c_str(),
									  pHook_in->m_sFuncName.c_str(),
									  pHook_in->m_pTrampolineFunc);
	}
}

#endif//__USE_IATHOOK