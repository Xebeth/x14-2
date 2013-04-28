/**************************************************************************
	created		:	2011-06-02
	filename	: 	MinHookManager.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Hook manager using MinHook
**************************************************************************/
#include "stdafx.h"
#ifdef __USE_MINHOOK

#include <MinHook.h>

#include "HookManager.h"
#include "MinHookManager.h"

#include "Hook.h"

#ifdef _DEBUG
	#ifdef _M_X64
		#pragma comment(lib, "libMinHook.x64d.lib")
	#else
		#pragma comment(lib, "libMinHook.x86d.lib")
	#endif
#else
	#ifdef _M_X64
		#pragma comment(lib, "libMinHook.x64.lib")
	#else
		#pragma comment(lib, "libMinHook.x86.lib")
	#endif
#endif // _DEBUG

namespace HookEngineLib
{
	/*! \brief Initializes the hooking process
		\return true if the initialization succeeded; false otherwise
	*/
	bool MinHookManager::Initialize()
	{
		m_bInit = (MH_Initialize() == MH_OK);

		return m_bInit;
	}

	/*! \brief Terminates the hooking process
		\return true if the termination succeeded; false otherwise
	*/
	void MinHookManager::Shutdown()
	{
		IHookManager::Shutdown();
		m_bInit = !(MH_Uninitialize() == MH_OK);
	}

	/*! \brief Creates a hook by patching the original function
		\param[in] pHook_in : the hook to create
		\return true if the hook was created; false otherwise
	*/
	bool MinHookManager::CreateHook(Hook *pHook_in_out)
	{
		// use Detours to install the hook
		return (MH_CreateHook(pHook_in_out->m_pOriginalFunc, pHook_in_out->m_pHookFunc,
							  &pHook_in_out->m_pTrampolineFunc) == MH_OK
			 && MH_EnableHook(pHook_in_out->m_pOriginalFunc) == MH_OK);
	}

	/*! \brief Destroys a hook by restoring the original function
		\param[in] pHook_in : the hook to destroy
		\return true if the hook was destroyed; false otherwise
	*/
	bool MinHookManager::DestroyHook(const Hook *pHook_in)
	{
		// use Detours to remove the hook
		return (MH_DisableHook(pHook_in->m_pOriginalFunc) == MH_OK);
	}
}

#endif//__USE_MINHOOK