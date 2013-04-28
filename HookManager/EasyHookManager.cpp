/**************************************************************************
	created		:	2011-06-02
	filename	: 	EasyHookManager.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Hook manager using EasyHook
**************************************************************************/
#include "stdafx.h"
#ifdef __USE_EASYHOOK

#include <EasyHook.h>

#include "HookManager.h"
#include "EasyHookManager.h"

#include "Hook.h"

#ifdef _M_X64
	#pragma comment(lib, "EasyHookStatic64.lib")
#else
	#pragma comment(lib, "EasyHookStatic32.lib")
#endif

namespace HookEngineLib
{
	/*! \brief Initializes the hooking process
		\return true if the initialization succeeded; false otherwise
	*/
	bool EasyHookManager::Initialize()
	{
		m_bInit = (RhInstallSupportDriver() == S_OK);

		return m_bInit;
	}

	/*! \brief Terminates the hooking process
		\return true if the termination succeeded; false otherwise
	*/
	void EasyHookManager::Shutdown()
	{
		IHookManager::Shutdown();
		m_bInit = !(LhWaitForPendingRemovals() == S_OK);
	}

	/*! \brief Creates a hook by patching the original function
		\param[in] pHook_in : the hook to create
		\return true if the hook was created; false otherwise
	*/
	bool EasyHookManager::CreateHook(Hook *pHook_in_out)
	{
		// use Detours to install the hook
		return (LhInstallHook(pHook_in_out->m_pOriginalFunc, pHook_in_out->m_pHookFunc,
							  pHook_in_out->m_pTrampolineFunc, &m_hHook) == S_OK);
	}

	/*! \brief Destroys a hook by restoring the original function
		\param[in] pHook_in : the hook to destroy
		\return true if the hook was destroyed; false otherwise
	*/
	bool EasyHookManager::DestroyHook(const Hook *pHook_in)
	{
		// use Detours to remove the hook
		return (LhUninstallHook(&m_hHook) == S_OK);
	}
}

#endif//__USE_EASYHOOK