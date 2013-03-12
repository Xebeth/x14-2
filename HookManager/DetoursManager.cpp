/**************************************************************************
	created		:	2011-06-02
	filename	: 	DetoursManager.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Hook manager using Microsoft Detours 1.2
**************************************************************************/
#include "stdafx.h"
#ifdef __USE_DETOURS

#include <detours.h>

#include "HookManager.h"
#include "DetoursManager.h"

#pragma comment(lib, "detours.lib")

namespace HookEngineLib
{
	/*! \brief Initializes the installation process
		\return true if the initialization succeeded; false otherwise
	*/
	bool DetoursManager::BeginTransaction()
	{
		if (::DetourTransactionBegin() == NO_ERROR)
			return (::DetourUpdateThread(::GetCurrentThread()) == NO_ERROR);

		return false;
	}

	/*! \brief Commits the installation process
		\return true if the commit succeeded; false otherwise
	*/
	bool DetoursManager::CommitTransaction()
	{
		return (DetourTransactionCommit() == NO_ERROR);
	}

	/*! \brief Creates a hook by patching the original function
		\param[in] pHook_in : the hook to create
		\return true if the hook was created; false otherwise
	*/
	bool DetoursManager::CreateHook(Hook *pHook_in_out)
	{
		// use Detours to install the hook
		return (DetourAttachEx(&(LPVOID&)pHook_in_out->m_pOriginalFunc, pHook_in_out->m_pHookFunc,
							   &(DETOUR_TRAMPOLINE*&)pHook_in_out->m_pTrampolineFunc, NULL, NULL) == NO_ERROR);
	}

	/*! \brief Destroys a hook by restoring the original function
		\param[in] pHook_in : the hook to destroy
		\return true if the hook was destroyed; false otherwise
	*/
	bool DetoursManager::DestroyHook(const Hook *pHook_in)
	{
		// use Detours to remove the hook
		return (DetourDetach(&(LPVOID&)pHook_in->m_pOriginalFunc, 
							 pHook_in->m_pHookFunc) == NO_ERROR);
	}
}

#endif//__USE_DETOURS