/**************************************************************************
	created		:	2011-06-02
	filename	: 	DetoursManager.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Hook manager using Microsoft Detours 1.2
**************************************************************************/
#include "stdafx.h"
#include "HookEngine.h"

#ifdef _DEBUG
	#pragma comment(lib, "detours_d.lib")
#else
	#pragma comment(lib, "detours.lib")
#endif // _DEBUG

namespace HookEngineLib
{
	/*! \brief Installs all the hooks currently registered in the manager
		\return true if all the hooks were installed successfully; false otherwise
	*/
	bool DetoursManager::InstallRegisteredHooks()
	{
		HookPtrMap::const_iterator Iter;

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		for (Iter = m_HookMap.begin(); Iter != m_HookMap.end(); ++Iter)
			if (Iter->second != NULL && Iter->second->m_bInstalled == false)
				InstallHook(Iter->second);

		return (DetourTransactionCommit() == NO_ERROR);
	}

	/*! \brief Uninstalls all the hooks currently registered in the manager
		\return true if all the hooks were uninstalled successfully; false otherwise
	*/
	bool DetoursManager::UninstallRegisteredHooks()
	{
		HookPtrMap::const_iterator Iter;

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		for (Iter = m_HookMap.begin(); Iter != m_HookMap.end(); ++Iter)
			if (Iter->second != NULL && Iter->second->m_bInstalled)
				UninstallHook(Iter->second);

		return (DetourTransactionCommit() == NO_ERROR);
	}

	/*! \brief Installs the specified hook using Detours
		\param[in,out] pHook_in_out : a pointer to the hook to be installed
		\return true if successful; false otherwise
	*/
	bool DetoursManager::InstallHook(Hook *pHook_in_out)
	{
		if (pHook_in_out != NULL)
		{
			// if the original function was left NULL
			if (pHook_in_out->m_pOriginalFunc == NULL)
			{
				// try to find it in the module
				pHook_in_out->m_pOriginalFunc = DetourFindFunction(pHook_in_out->m_sModuleName.c_str(),
																   pHook_in_out->m_sFuncName.c_str());
			}

			if (pHook_in_out->m_pOriginalFunc != NULL && pHook_in_out->m_pHookFunc != NULL)
			{
				// if the hook isn't already installed
				if (pHook_in_out->m_bInstalled == false)
				{
					// if we're not hooking a class member
					if (pHook_in_out->m_dwOpCodesSize == 0)
					{
						// use Detours to install the hook
						LONG AttachResult = DetourAttachEx(&(LPVOID&)pHook_in_out->m_pOriginalFunc, pHook_in_out->m_pHookFunc,
														   &(DETOUR_TRAMPOLINE*&)pHook_in_out->m_pTrampolineFunc, NULL, NULL);
						// flag the hook has installed if no error occurred
						pHook_in_out->m_bInstalled = (AttachResult == NO_ERROR);
					}
					else
					{
						// use DetourClassFunc (Azorbix@Game Deception)
						pHook_in_out->m_pTrampolineFunc = DetourClassFunc((LPBYTE)pHook_in_out->m_pOriginalFunc,
																		  (LPBYTE)pHook_in_out->m_pHookFunc,
																		  pHook_in_out->m_dwOpCodesSize);
						// flag the hook has been installed (no real way to check success)
						pHook_in_out->m_bInstalled = (pHook_in_out->m_pTrampolineFunc != NULL);
					}
				}

				return pHook_in_out->m_bInstalled;
			}
		}

		return false;
	}

	/*! \brief Uninstalls the specified hook using Detours
		\param[in,out] pHook_in_out : a pointer to the hook to be uninstalled
		\return true if successful; false otherwise
	*/
	bool DetoursManager::UninstallHook(Hook *pHook_in_out)
	{
		if (pHook_in_out != NULL)
		{
			if (pHook_in_out->m_pOriginalFunc != NULL && pHook_in_out->m_pHookFunc != NULL)
			{
				// if the hook is installed
				if (pHook_in_out->m_bInstalled)
				{
					// if we're not unhooking a class member
					if (pHook_in_out->m_dwOpCodesSize == 0)
					{
						// use Detours to remove the hook
						LONG DetachResult = DetourDetach(&(LPVOID&)pHook_in_out->m_pOriginalFunc, pHook_in_out->m_pHookFunc);
						// flag the hook has uninstalled if no error occurred
						pHook_in_out->m_bInstalled = !(DetachResult == NO_ERROR);
					}
					else
					{
						// use RetourClassFunc (Azorbix@Game Deception)
						RetourClassFunc((LPBYTE)pHook_in_out->m_pOriginalFunc,
										(LPBYTE)pHook_in_out->m_pTrampolineFunc,
										pHook_in_out->m_dwOpCodesSize);
						// flag the hook has been uninstalled (no real way to check success)
						pHook_in_out->m_bInstalled = false;
					}
				}

				return !pHook_in_out->m_bInstalled;
			}
		}

		return false;
	}
}