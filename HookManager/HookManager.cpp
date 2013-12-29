/**************************************************************************
	created		:	2011-06-02
	filename	: 	HookManager.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <algorithm>

#include "HookManager.h"
#include "Hook.h"

namespace HookEngineLib
{
	//! \brief IHookManager destructor
	IHookManager::~IHookManager()
	{
		MemberDetours::const_iterator DetourIt, DetourEndIt = m_AsmData.cend();
		HookPtrMap::const_iterator HookIt, HookEndIt = m_HookMap.cend();

		for (DetourIt = m_AsmData.cbegin(); DetourIt != DetourEndIt; ++DetourIt)
			delete DetourIt->second;

		for (HookIt = m_HookMap.cbegin(); HookIt != HookEndIt; ++HookIt)
			delete HookIt->second;
	}

	/*! \brief Installs all the hooks currently registered in the manager
		\return true if all the hooks were installed successfully; false otherwise
	*/
	bool IHookManager::InstallRegisteredHooks()
	{
		if (m_bInit == false)
			m_bInit = Initialize();

		if (m_bInit && BeginTransaction())
		{
			HookPtrMap::const_iterator HookIt, HookEndIt = m_HookMap.cend();
			bool bResult = true;
			Hook *pHook = NULL;

			for (HookIt = m_HookMap.cbegin(); HookIt != HookEndIt; ++HookIt)
			{
				pHook = HookIt->second;

				if (pHook != NULL && pHook->m_bInstalled == false)
					bResult &= (InstallHook(pHook) != NULL);
			}

			return (CommitTransaction() && bResult);
		}

		return false;
	}

	/*! \brief Uninstalls all the hooks currently registered in the manager
		\return true if all the hooks were uninstalled successfully; false otherwise
	*/
	bool IHookManager::UnregisterHooks()
	{
		if (m_bInit && BeginTransaction())
		{
			HookPtrMap::const_iterator HookIt = m_HookMap.cbegin();
			HookPtrMap::const_iterator EndIt = m_HookMap.cend();
			bool bResult = true;
			Hook *pHook = NULL;

			while (HookIt != EndIt)
			{
				pHook = HookIt->second;

				if (pHook != NULL && UnregisterHook(HookIt) == false)
				{
					bResult = false;
					++HookIt;
				}
			}

			return (CommitTransaction() && bResult);
		}

		return false;
	}

	/*! \brief Installs the specified hook using Detours
		\param[in,out] pHook_in_out : a pointer to the hook to be installed
		\return true if successful; false otherwise
	*/
	LPVOID IHookManager::InstallHook(Hook *pHook_in_out)
	{
		if (pHook_in_out != NULL)
		{
			if (pHook_in_out->m_pOriginalFunc == NULL)
			{
				// try to find the function automatically
				pHook_in_out->m_pOriginalFunc = FindModuleFunction(pHook_in_out->m_sModuleName.c_str(),
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
						// flag the hook has installed if no error occurred
						pHook_in_out->m_bInstalled = CreateHook(pHook_in_out);
					}
					else
					{
						// use DetourClassFunc
						pHook_in_out->m_pTrampolineFunc = DetourClassFunc((LPBYTE)pHook_in_out->m_pOriginalFunc,
																		  (LPBYTE)pHook_in_out->m_pHookFunc,
																		  pHook_in_out->m_dwOpCodesSize);
						// flag the hook has been installed (no real way to check success)
						pHook_in_out->m_bInstalled = (pHook_in_out->m_pTrampolineFunc != NULL);
					}
				}
			}

			return pHook_in_out->m_pTrampolineFunc;
		}

		return NULL;
	}

	/*! \brief Uninstalls the specified hook using Detours
		\param[in,out] pHook_in_out : a pointer to the hook to be uninstalled
		\return true if successful; false otherwise
	*/
	bool IHookManager::UninstallHook(Hook *pHook_in_out)
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
						// flag the hook has uninstalled if no error occurred
						pHook_in_out->m_bInstalled = DestroyHook(pHook_in_out);
					}
					else
					{
						// use RetourClassFunc to restore the original function
						pHook_in_out->m_bInstalled = !RetourClassFunc((LPBYTE)pHook_in_out->m_pOriginalFunc,
																	  pHook_in_out->m_dwOpCodesSize);
					}
				}

				return !pHook_in_out->m_bInstalled;
			}
		}

		return false;
	}

	/*! \brief Registers a new hook
		\param[in] pFuncName_in : the name of the target function
		\param[in] pModuleName_in : the name of the module containing the hooked function
		\param[in] pOriginalFunc_in : function pointer to the target function
		\param[in] pHookFunc_in : function pointer to the replacement function
		\param[in] dwOpCodeSize_in : the size of the instructions being overwritten for class member hook
	*/
	void IHookManager::RegisterHook(const char* pFuncName_in, const char* pModuleName_in, LPVOID pOriginalFunc_in,
									LPVOID pHookFunc_in, DWORD dwOpCodeSize_in)
	{
		HookPtrMap::const_iterator HookIt = m_HookMap.find(pFuncName_in);

		// clean up the previous instance
		if (HookIt != m_HookMap.cend() && HookIt->second != NULL)
			UninstallHook(HookIt->second);

		m_HookMap[pFuncName_in] = new Hook(pFuncName_in, pModuleName_in, pOriginalFunc_in, pHookFunc_in, dwOpCodeSize_in);
	}

	/*! \brief Registers a new hook given a memory signature
		\param[in] pFuncName_in : the name of the target function
		\param[in] pModuleName_in : the name of the module containing the hooked function
		\param[in] pPattern_in : 
		\param[in] Offset_in : 
		\param[in] pHookFunc_in : function pointer to the replacement function
		\param[in] dwOpCodeSize_in : the size of the instructions being overwritten for class member hook
	*/
	void IHookManager::RegisterHook(const char* pFuncName_in, const char* pModuleName_in,
									const char* pPattern_in, int Offset_in, 
									LPVOID pHookFunc_in, DWORD dwOpCodeSize_in)
	{
		if (pPattern_in != NULL && pModuleName_in != NULL && pFuncName_in != NULL && pHookFunc_in != NULL)
		{
			std::string ModuleName(pModuleName_in);
			string_t ProcessName;

			// convert the process name to ANSI
			convert_utf8(ModuleName, ProcessName);

			if (m_MemScan.Initialize(::GetCurrentProcessId(), ProcessName.c_str()))
			{
				// scan for the pattern
				DWORD_PTR dwFuncAddr = m_MemScan.ScanCode(pPattern_in, Offset_in);

				if (dwFuncAddr != NULL)
					RegisterHook(pFuncName_in, pModuleName_in, (LPVOID)dwFuncAddr, pHookFunc_in, dwOpCodeSize_in);
			}
		}
	}

	/*! \brief Unregisters a hook given the hook name
		\param[in] pFuncName_in : the name of the hook
	*/
	void IHookManager::UnregisterHook(const char* pFuncName_in)
	{
		HookPtrMap::const_iterator HookIt = m_HookMap.find(pFuncName_in);

		if (HookIt != m_HookMap.cend())
			UnregisterHook(HookIt);
	}

	/*! \brief Unregisters a hook given its position in the map
		\param[in] Iter_in_out : the position of the hook in the map
	*/
	bool IHookManager::UnregisterHook(HookPtrMap::const_iterator &Iter_in_out)
	{
		if (Iter_in_out != m_HookMap.cend())
		{
			Hook *pHook = Iter_in_out->second;

			if (pHook != NULL && pHook->m_bInstalled)
				return UninstallHook(pHook);
		}

		return false;
	}

	/*! \brief Retrieves the original function given the hook name
		\param[in] pFuncName_in : the name of the hook
		\return a function pointer to original function
	*/
	LPVOID IHookManager::GetOriginalFunc(const char* pFuncName_in) const
	{
		HookPtrMap::const_iterator HookIt = m_HookMap.find(pFuncName_in);

		if (HookIt != m_HookMap.cend() && HookIt->second != NULL)
			return HookIt->second->m_pOriginalFunc;
		else
			return NULL;
	}

	/*! \brief Retrieves the hook function given the hook name
		\param[in] pFuncName_in : the name of the hook
		\return a function pointer to hook function
	*/
	LPVOID IHookManager::GetHookFunc(const char* pFuncName_in) const
	{
		HookPtrMap::const_iterator HookIt = m_HookMap.find(pFuncName_in);

		if (HookIt != m_HookMap.cend() && HookIt->second != NULL)
			return HookIt->second->m_pHookFunc;
		else
			return NULL;
	}

	/*! \brief Retrieves the trampoline function given the hook name
		\param[in] pFuncName_in : the name of the hook
		\return a function pointer to trampoline function
	*/
	LPVOID IHookManager::GetTrampolineFunc(const char* pFuncName_in) const
	{
		HookPtrMap::const_iterator HookIt = m_HookMap.find(pFuncName_in);

		if (HookIt != m_HookMap.cend() && HookIt->second != NULL)
			return HookIt->second->m_pTrampolineFunc;
		else
			return NULL;
	}

	/*! \brief Installs a hook given its name
		\param[in] pFuncName_in : the name of the hook
	*/
	LPVOID IHookManager::InstallHook(const char* pFuncName_in)
	{
		HookPtrMap::const_iterator HookIt = m_HookMap.find(pFuncName_in);

		if (HookIt != m_HookMap.cend() && HookIt->second != NULL)
			return InstallHook(HookIt->second);

		return NULL;
	}

	/*! \brief Checks if the hook specified by the function name is installed
		\param[in] pFuncName_in : the name of the hook
		\return true if the hook is installed; false otherwise
	*/
	bool IHookManager::IsHookInstalled(const char* pFuncName_in)
	{
		HookPtrMap::const_iterator HookIt = m_HookMap.find(pFuncName_in);

		return (HookIt != m_HookMap.cend() && HookIt->second->m_bInstalled);
	}

	/*! \brief Checks if the hook specified by the function name is registered
		\param[in] pFuncName_in : the name of the hook
		\return true if the hook is registered; false otherwise
	*/
	bool IHookManager::IsHookRegistered(const char* pFuncName_in)
	{
		HookPtrMap::const_iterator HookIt = m_HookMap.find(pFuncName_in);

		return (HookIt != m_HookMap.cend());
	}


	/*! \brief Uninstall a hook given its name
		\param[in] pFuncName_in : the name of the hook
	*/
	bool IHookManager::UninstallHook(const char* pFuncName_in)
	{
		HookPtrMap::const_iterator HookIt = m_HookMap.find(pFuncName_in);

		if (HookIt != m_HookMap.cend() && HookIt->second != NULL)
			return UninstallHook(HookIt->second);

		return false;
	}

	/*! \brief Hooks a class member with an __stdcall function by pushing ECX (this) on the stack.
		\param[in] pSrc_in : the address of the function to be hooked
		\param[in] pDst_in : an __stdcall hook function
		\param[in] OpCodesSize_in : the size of the first byte of the original function
									it must include the size of the byte codes comprising
									the instructions so at least 8 bytes are overwritten
		\return the trampoline (detoured) function pointer
	*/
	LPVOID IHookManager::DetourClassFunc(LPBYTE pSrc_in, const LPBYTE pDst_in, DWORD OpCodesSize_in)
	{
		LPVOID pTrampoline = NULL;

		if (OpCodesSize_in >= TRAMPOLINE_OPCODES_SIZE)
		{
			DWORD dwPageAccess;

			if (VirtualProtect(pSrc_in, OpCodesSize_in, PAGE_READWRITE, &dwPageAccess))
			{
				MemberDetourAsm *pDetour = new MemberDetourAsm;

				if (pDetour->SetDetour(pSrc_in, pDst_in, OpCodesSize_in))
				{
					DWORD dwDummyAccess;

					pTrampoline = pDetour->GetTrampoline();
					m_AsmData[pSrc_in] = pDetour;

					VirtualProtect(pTrampoline, OpCodesSize_in, PAGE_EXECUTE_READWRITE, &dwDummyAccess);
				}
				
				VirtualProtect(pSrc_in, OpCodesSize_in, dwPageAccess, &dwPageAccess);
			}
		}

		return pTrampoline;
	}

	/*! \brief Restores a hook class member function
		\param[in] pSrc_in : the byte codes of the original function
	*/
	bool IHookManager::RetourClassFunc(const LPBYTE pSrc_in, DWORD OpCodesSize_in)
	{
		MemberDetours::const_iterator DetourIt = m_AsmData.find(pSrc_in);

		if (DetourIt != m_AsmData.cend())
		{
			MemberDetourAsm *pDetour = DetourIt->second;

			if (pDetour != NULL)
			{
				DWORD dwPageAccess;

				VirtualProtect(pSrc_in, OpCodesSize_in, PAGE_READWRITE, &dwPageAccess);

				// restore the original function
				pDetour->SetRetour(pSrc_in, OpCodesSize_in);

				VirtualProtect(pSrc_in, OpCodesSize_in, dwPageAccess, &dwPageAccess);

				return true;
			}
		}

		return false;
	}

	/*! \brief Finds a function within the specified module
		\param[in] pModuleName_in : the name of the module exporting the function
		\param[in] pFuncName_in : the name of the function to find
		\return the address of the function if found; NULL otherwise
	*/
	LPVOID IHookManager::FindModuleFunction(const char *pModuleName_in, const char *pFuncName_in)
	{
		if (pFuncName_in != NULL && pModuleName_in != NULL)
		{
			HMODULE hModule = ::LoadLibraryExA(pModuleName_in, NULL, 0);

			if (hModule != NULL)
				return ::GetProcAddress(hModule, pFuncName_in);
		}

		return NULL;
	}

	SigScan::SigScan& IHookManager::GetSigScan()
	{ return m_MemScan; }

	//! \brief Terminates the hooking process
	void IHookManager::Shutdown()
	{
		UnregisterHooks();
		m_bInit = false;
	}
}