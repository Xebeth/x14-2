/**************************************************************************
	created		:	2011-06-15
	filename	: 	IATHookManager.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Hook manager using IAT Patcher
**************************************************************************/
#include "stdafx.h"
#include "HookEngine.h"

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
	bool IATHookManager::InstallRegisteredHooks()
	{
		HookPtrIterator Iter;
		bool bResult = true;

		if (m_bInit == false)
		{
			m_hProcess = GetModuleHandle(NULL);
			m_bInit = (m_hProcess != NULL);
		}

		if (m_bInit)
		{
			for (Iter = m_HookMap.begin(); Iter != m_HookMap.end(); ++Iter)
				if (Iter->second != NULL && Iter->second->m_bInstalled == false)
					bResult &= InstallHook(Iter->second);
		}

		return (m_bInit && bResult);
	}

	bool IATHookManager::UninstallRegisteredHooks()
	{
		HookPtrIterator Iter;
		bool bResult = true;

		if (m_bInit)
		{
			for (Iter = m_HookMap.begin(); Iter != m_HookMap.end(); ++Iter)
				if (Iter->second != NULL && Iter->second->m_bInstalled)
					bResult &= UninstallHook(Iter->second);

			m_bInit = false;
			m_hProcess = NULL;
		}

		return (!m_bInit && bResult);
	}

	bool IATHookManager::InstallHook(Hook *pHook)
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
					pHook->m_bInstalled = IATPatcher::PatchIAT(m_hProcess, pHook->m_sModuleName.c_str(),
															   pHook->m_sFuncName.c_str(),
															   &pHook->m_pTrampolineFunc,
															   pHook->m_pHookFunc);
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

	bool IATHookManager::UninstallHook(Hook *pHook)
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
					pHook->m_bInstalled = !IATPatcher::RestoreIAT(m_hProcess, pHook->m_sModuleName.c_str(),
																  pHook->m_sFuncName.c_str(),
																  pHook->m_pTrampolineFunc);
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