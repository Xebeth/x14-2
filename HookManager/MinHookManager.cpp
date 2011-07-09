/**************************************************************************
	created		:	2011-06-02
	filename	: 	MinHookManager.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Hook manager using MinHook
**************************************************************************/
#include "stdafx.h"
#include "HookEngine.h"

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
	bool MinHookManager::InstallRegisteredHooks()
	{
		HookPtrIterator Iter;
		bool bResult = true;

		if (m_bInit == false)
			m_bInit = (MH_Initialize() == MH_OK);

		if (m_bInit)
		{
			for (Iter = m_HookMap.begin(); Iter != m_HookMap.end(); ++Iter)
				if (Iter->second != NULL && Iter->second->m_bInstalled == false)
					bResult &= InstallHook(Iter->second);
		}

		return (m_bInit && bResult);
	}

	bool MinHookManager::UninstallRegisteredHooks()
	{
		HookPtrIterator Iter;
		bool bResult = true;

		if (m_bInit)
		{
			for (Iter = m_HookMap.begin(); Iter != m_HookMap.end(); ++Iter)
				if (Iter->second != NULL && Iter->second->m_bInstalled)
					bResult &= UninstallHook(Iter->second);

			m_bInit = !(MH_Uninitialize() == MH_OK);
		}

		return (!m_bInit && bResult);
	}

	/*! \brief Installs the specified hook using MinHook
		\param[in,out] pHook : a pointer to the hook to be installed
		\return true if successful; false otherwise
	*/
	bool MinHookManager::InstallHook(Hook *pHook)
	{
		if (pHook != NULL && pHook->m_pOriginalFunc != NULL && pHook->m_pHookFunc != NULL)
		{
			// if the hook isn't already installed
			if (pHook->m_bInstalled == false)
			{
				// if we're not hooking a class member
				if (pHook->m_dwOpCodesSize == 0)
				{
					// flag the hook has installed if no error occurred
					pHook->m_bInstalled = (MH_CreateHook(pHook->m_pOriginalFunc, pHook->m_pHookFunc, &pHook->m_pTrampolineFunc) == MH_OK);
					pHook->m_bInstalled &= (MH_EnableHook(pHook->m_pOriginalFunc) == MH_OK);
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

		return false;
	}

	/*! \brief Uninstalls the specified hook using MinHook
		\param[in,out] pHook : a pointer to the hook to be uninstalled
		\return true if successful; false otherwise
	*/
	bool MinHookManager::UninstallHook(Hook *pHook)
	{
		if (pHook != NULL && pHook->m_pOriginalFunc != NULL && pHook->m_pHookFunc != NULL)
		{
			// if the hook is installed
			if (pHook->m_bInstalled)
			{
				// if we're not unhooking a class member
				if (pHook->m_dwOpCodesSize == 0)
				{
					// flag the hook has uninstalled if no error occurred
					pHook->m_bInstalled = !(MH_DisableHook(pHook->m_pOriginalFunc) == MH_OK);
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

		return false;
	}
}