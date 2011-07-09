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
	bool DetoursManager::InstallRegisteredHooks()
	{
		HookPtrIterator Iter;

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		for (Iter = m_HookMap.begin(); Iter != m_HookMap.end(); ++Iter)
			if (Iter->second != NULL && Iter->second->m_bInstalled == false)
				InstallHook(Iter->second);

		return (DetourTransactionCommit() == NO_ERROR);
	}

	bool DetoursManager::UninstallRegisteredHooks()
	{
		HookPtrIterator Iter;

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		for (Iter = m_HookMap.begin(); Iter != m_HookMap.end(); ++Iter)
			if (Iter->second != NULL && Iter->second->m_bInstalled)
				UninstallHook(Iter->second);

		return (DetourTransactionCommit() == NO_ERROR);
	}

	/*! \brief Installs the specified hook using Detours
		\param[in,out] pHook : a pointer to the hook to be installed
		\return true if successful; false otherwise
	*/
	bool DetoursManager::InstallHook(Hook *pHook)
	{
		if (pHook != NULL)
		{
			// if the original function was left NULL
			if (pHook->m_pOriginalFunc == NULL)
			{
				// try to find it in the module
				pHook->m_pOriginalFunc = DetourFindFunction(pHook->m_sModuleName.c_str(),
															pHook->m_sFuncName.c_str());
			}

			if (pHook->m_pOriginalFunc != NULL && pHook->m_pHookFunc != NULL)
			{
				// if the hook isn't already installed
				if (pHook->m_bInstalled == false)
				{
					// if we're not hooking a class member
					if (pHook->m_dwOpCodesSize == 0)
					{
						// use Detours to install the hook
						LONG AttachResult = DetourAttachEx(&(LPVOID&)pHook->m_pOriginalFunc, pHook->m_pHookFunc,
														   &(DETOUR_TRAMPOLINE*&)pHook->m_pTrampolineFunc, NULL, NULL);
						// flag the hook has installed if no error occurred
						pHook->m_bInstalled = (AttachResult == NO_ERROR);
					}
					else
					{
						// use DetourClassFunc (Azorbix@Game Deception)
						pHook->m_pTrampolineFunc = DetourClassFunc((LPBYTE)pHook->m_pOriginalFunc,
																   (LPBYTE)pHook->m_pHookFunc,
																   pHook->m_dwOpCodesSize);
						// flag the hook has been installed (no real way to check success)
						pHook->m_bInstalled = (pHook->m_pTrampolineFunc != NULL);
					}
				}

				return pHook->m_bInstalled;
			}
		}

		return false;
	}

	/*! \brief Uninstalls the specified hook using Detours
		\param[in,out] pHook : a pointer to the hook to be uninstalled
		\return true if successful; false otherwise
	*/
	bool DetoursManager::UninstallHook(Hook *pHook)
	{
		if (pHook != NULL)
		{
			if (pHook->m_pOriginalFunc != NULL && pHook->m_pHookFunc != NULL)
			{
				// if the hook is installed
				if (pHook->m_bInstalled)
				{
					// if we're not unhooking a class member
					if (pHook->m_dwOpCodesSize == 0)
					{
						// use Detours to remove the hook
						LONG DetachResult = DetourDetach(&(LPVOID&)pHook->m_pOriginalFunc, pHook->m_pHookFunc);
						// flag the hook has uninstalled if no error occurred
						pHook->m_bInstalled = !(DetachResult == NO_ERROR);
					}
					else
					{
						// use RetourClassFunc (Azorbix@Game Deception)
						RetourClassFunc((LPBYTE)pHook->m_pOriginalFunc,
										(LPBYTE)pHook->m_pTrampolineFunc,
										pHook->m_dwOpCodesSize);
						// flag the hook has been uninstalled (no real way to check success)
						pHook->m_bInstalled = false;
					}
				}

				return !pHook->m_bInstalled;
			}
		}

		return false;
	}
}