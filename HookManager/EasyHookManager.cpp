/**************************************************************************
	created		:	2011-06-02
	filename	: 	EasyHookManager.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Hook manager using EasyHook
**************************************************************************/
#include "stdafx.h"
#include "HookEngine.h"

#ifdef _M_X64
	#pragma comment(lib, "EasyHookStatic64.lib")
#else
	#pragma comment(lib, "EasyHookStatic32.lib")
#endif

namespace HookEngineLib
{
	/*! \brief Installs all the hooks currently registered in the manager
		\return true if all the hooks were installed successfully; false otherwise
	*/
	bool EasyHookManager::InstallRegisteredHooks()
	{
		HookPtrMap::const_iterator Iter;
		bool bResult = true;

		if (m_bInit == false)
			m_bInit = (RhInstallSupportDriver() == S_OK);

		for (Iter = m_HookMap.begin(); Iter != m_HookMap.end(); ++Iter)
			if (Iter->second != NULL && Iter->second->m_bInstalled == false)
				bResult &= InstallHook(Iter->second);

		return (m_bInit && bResult);
	}

	/*! \brief Uninstalls all the hooks currently registered in the manager
		\return true if all the hooks were uninstalled successfully; false otherwise
	*/
	bool EasyHookManager::UninstallRegisteredHooks()
	{
		HookPtrMap::const_iterator Iter;
		bool bResult = true;

		if (m_bInit)
		{
			for (Iter = m_HookMap.begin(); Iter != m_HookMap.end(); ++Iter)
				if (Iter->second != NULL && Iter->second->m_bInstalled)
					bResult &= UninstallHook(Iter->second);

			m_bInit = (LhWaitForPendingRemovals() == S_OK);
		}

		return (m_bInit && bResult);
	}

	/*! \brief Installs the specified hook using EasyHook
		\param[in,out] pHook_in_out : a pointer to the hook to be installed
		\return true if successful; false otherwise
	*/
	bool EasyHookManager::InstallHook(Hook *pHook_in_out)
	{
		if (pHook_in_out != NULL && pHook_in_out->m_pOriginalFunc != NULL && pHook_in_out->m_pHookFunc != NULL)
		{
			// if the hook isn't already installed
			if (pHook_in_out->m_bInstalled == false)
			{
				// if we're not hooking a class member
				if (pHook_in_out->m_dwOpCodesSize == 0)
				{
					// flag the hook has installed if no error occurred
					pHook_in_out->m_bInstalled = (LhInstallHook(pHook_in_out->m_pOriginalFunc, pHook_in_out->m_pHookFunc, pHook_in_out->m_pTrampolineFunc, &m_hHook) == S_OK);
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

		return false;
	}

	/*! \brief Uninstalls the specified hook using EasyHook
		\param[in,out] pHook_in_out : a pointer to the hook to be uninstalled
		\return true if successful; false otherwise
	*/
	bool EasyHookManager::UninstallHook(Hook *pHook_in_out)
	{
		if (pHook_in_out != NULL && pHook_in_out->m_pOriginalFunc != NULL && pHook_in_out->m_pHookFunc != NULL)
		{
			// if the hook is installed
			if (pHook_in_out->m_bInstalled)
			{
				// if we're not unhooking a class member
				if (pHook_in_out->m_dwOpCodesSize == 0)
				{
					// flag the hook has uninstalled if no error occurred
					pHook_in_out->m_bInstalled = !(LhUninstallHook(&m_hHook) == S_OK);
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

		return false;
	}
}