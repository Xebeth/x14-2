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

namespace HookEngineLib
{
	//! \brief IHookManager destructor
	IHookManager::~IHookManager()
	{
		UninstallRegisteredHooks();
		Shutdown();

		HookPtrMap::iterator Iter;

		for (Iter = m_HookMap.begin(); Iter != m_HookMap.end(); ++Iter)
			UnregisterHook(Iter);

		AsmData::iterator AsmIter;

		for (AsmIter = m_AsmData.begin(); AsmIter != m_AsmData.end(); ++AsmIter)
			free(*AsmIter);
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
			HookPtrMap::const_iterator Iter;
			bool bResult = true;

			for (Iter = m_HookMap.begin(); Iter != m_HookMap.end(); ++Iter)
				if (Iter->second != NULL && Iter->second->m_bInstalled == false)
					bResult &= (InstallHook(Iter->second) != NULL);

			return (CommitTransaction() && bResult);
		}

		return false;
	}

	/*! \brief Uninstalls all the hooks currently registered in the manager
		\return true if all the hooks were uninstalled successfully; false otherwise
	*/
	bool IHookManager::UninstallRegisteredHooks()
	{
		if (m_bInit && BeginTransaction())
		{
			HookPtrMap::const_iterator Iter;
			bool bResult = true;

			for (Iter = m_HookMap.begin(); Iter != m_HookMap.end(); ++Iter)
				if (Iter->second != NULL && Iter->second->m_bInstalled)
					bResult &= UninstallHook(Iter->second);

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
		HookPtrMap::const_iterator Iter = m_HookMap.find(pFuncName_in);

		// clean up the previous instance
		if (Iter != m_HookMap.end() && Iter->second != NULL)
			UninstallHook(Iter->second);

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
			std::string Pattern(pPattern_in);			
			string_t ProcessName;

			// convert the process name to ANSI
			convert_utf8(ModuleName, ProcessName);
			// convert to upper case
			std::transform(Pattern.begin(), Pattern.end(), Pattern.begin(), ::toupper);

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
		HookPtrMap::iterator Iter = m_HookMap.find(pFuncName_in);

		if (Iter != m_HookMap.end())
			UnregisterHook(Iter);
	}

	/*! \brief Unregisters a hook given its position in the map
		\param[in] Iter_in : the position of the hook in the map
	*/
	void IHookManager::UnregisterHook(const HookPtrMap::iterator &Iter_in)
	{
		if (Iter_in != m_HookMap.end() && Iter_in->second != NULL)
		{
			if (Iter_in->second->m_bInstalled)
				UninstallHook(Iter_in->second);

			delete Iter_in->second;
			Iter_in->second = NULL;
		}
	}

	/*! \brief Retrieves the original function given the hook name
		\param[in] pFuncName_in : the name of the hook
		\return a function pointer to original function
	*/
	LPVOID IHookManager::GetOriginalFunc(const char* pFuncName_in) const
	{
		HookPtrMap::const_iterator Iter = m_HookMap.find(pFuncName_in);

		if (Iter != m_HookMap.end() && Iter->second != NULL)
			return Iter->second->m_pOriginalFunc;
		else
			return NULL;
	}

	/*! \brief Retrieves the hook function given the hook name
		\param[in] pFuncName_in : the name of the hook
		\return a function pointer to hook function
	*/
	LPVOID IHookManager::GetHookFunc(const char* pFuncName_in) const
	{
		HookPtrMap::const_iterator Iter = m_HookMap.find(pFuncName_in);

		if (Iter != m_HookMap.end() && Iter->second != NULL)
			return Iter->second->m_pHookFunc;
		else
			return NULL;
	}

	/*! \brief Retrieves the trampoline function given the hook name
		\param[in] pFuncName_in : the name of the hook
		\return a function pointer to trampoline function
	*/
	LPVOID IHookManager::GetTrampolineFunc(const char* pFuncName_in) const
	{
		HookPtrMap::const_iterator Iter = m_HookMap.find(pFuncName_in);

		if (Iter != m_HookMap.end() && Iter->second != NULL)
			return Iter->second->m_pTrampolineFunc;
		else
			return NULL;
	}

	/*! \brief Installs a hook given its name
		\param[in] pFuncName_in : the name of the hook
	*/
	LPVOID IHookManager::InstallHook(const char* pFuncName_in)
	{
		HookPtrMap::const_iterator Iter = m_HookMap.find(pFuncName_in);

		if (Iter != m_HookMap.end() && Iter->second != NULL)
			return InstallHook(Iter->second);

		return NULL;
	}

	/*! \brief Checks if the hook specified by the function name is installed
		\param[in] pFuncName_in : the name of the hook
		\return true if the hook is installed; false otherwise
	*/
	bool IHookManager::IsHookInstalled(const char* pFuncName_in)
	{
		HookPtrMap::const_iterator Iter = m_HookMap.find(pFuncName_in);

		return (Iter != m_HookMap.end() && Iter->second->m_bInstalled);
	}

	/*! \brief Checks if the hook specified by the function name is registered
		\param[in] pFuncName_in : the name of the hook
		\return true if the hook is registered; false otherwise
	*/
	bool IHookManager::IsHookRegistered(const char* pFuncName_in)
	{
		HookPtrMap::const_iterator Iter = m_HookMap.find(pFuncName_in);

		return (Iter != m_HookMap.end());
	}


	/*! \brief Uninstall a hook given its name
		\param[in] pFuncName_in : the name of the hook
	*/
	bool IHookManager::UninstallHook(const char* pFuncName_in)
	{
		HookPtrMap::const_iterator Iter = m_HookMap.find(pFuncName_in);

		if (Iter != m_HookMap.end() && Iter->second != NULL)
			return UninstallHook(Iter->second);

		return true;
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
		LPBYTE pTrampoline = (LPBYTE)malloc(OpCodesSize_in + 8);

		memset(pTrampoline, OPCODE_NOP, OpCodesSize_in + 8);

		if (pTrampoline != NULL)
		{
			DWORD dwPageAccess;

			VirtualProtect(pSrc_in, OpCodesSize_in, PAGE_READWRITE, &dwPageAccess);
			memcpy(pTrampoline + 3, pSrc_in, OpCodesSize_in);
			
			// write the hook code
			pTrampoline[0] = OPCODE_POP_EAX;						// POP EAX
			pTrampoline[1] = OPCODE_POP_ECX;						// POP ECX
			pTrampoline[2] = OPCODE_PUSH_EAX;						// PUSH EAX
			pTrampoline[OpCodesSize_in+3] = OPCODE_JMP;				// JMP
			// jump offset
			*(DWORD*)(pTrampoline+OpCodesSize_in+4) = (DWORD)((pSrc_in+OpCodesSize_in) - (pTrampoline+OpCodesSize_in+3)) - 5;

			// detour the source function call : push ECX (this) on the stack
			pSrc_in[0] = OPCODE_POP_EAX;							// POP EAX
			pSrc_in[1] = OPCODE_PUSH_ECX;							// PUSH ECX
			pSrc_in[2] = OPCODE_PUSH_EAX;							// PUSH EAX
			pSrc_in[3] = OPCODE_JMP;								// JMP
			// jump offset
			*(DWORD*)(pSrc_in+4) = (DWORD)(pDst_in - (pSrc_in+3)) - 5;

			for(DWORD i = 8; i < OpCodesSize_in; ++i)
				pSrc_in[i] = OPCODE_NOP;							// NOP

			VirtualProtect(pSrc_in, OpCodesSize_in, dwPageAccess, &dwPageAccess);
		}

		m_AsmData.push_back(pTrampoline);

		return pTrampoline;
	}

	/*! \brief Restores a hook class member function
		\param[in] pSrc_in : the byte codes of the original function
		\param[in] pTrampoline_in : the byte codes of the trampoline function
		\param[in] OpCodesSize_in : the size of the byte codes
	*/
	void IHookManager::RetourClassFunc(const LPBYTE pSrc_in, LPBYTE pTrampoline_in, DWORD OpCodesSize_in)
	{
		DWORD dwPageAccess;

		VirtualProtect(pSrc_in, OpCodesSize_in, PAGE_READWRITE, &dwPageAccess);

		// restore the original function
		memcpy(pSrc_in, pTrampoline_in+3, OpCodesSize_in);
		// jump back to the original function entry
		pTrampoline_in[3] = OPCODE_JMP;							// JMP
		// jump offset
		*(DWORD*)(pTrampoline_in+4) = (DWORD)(pSrc_in - (pTrampoline_in+3)) - 5;

		VirtualProtect(pSrc_in, OpCodesSize_in, dwPageAccess, &dwPageAccess);
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
}