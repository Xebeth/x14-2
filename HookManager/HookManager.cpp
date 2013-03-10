/**************************************************************************
	created		:	2011-06-02
	filename	: 	HookManager.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include "HookEngine.h"

namespace HookEngineLib
{
	//! \brief IHookManager destructor
	IHookManager::~IHookManager()
	{
		HookPtrMap::iterator Iter;

		for (Iter = m_HookMap.begin(); Iter != m_HookMap.end(); ++Iter)
			UnregisterHook(Iter);

		AsmData::iterator AsmIter;

		for (AsmIter = m_AsmData.begin(); AsmIter != m_AsmData.end(); ++AsmIter)
			free(*AsmIter);
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
	bool IHookManager::InstallHook(const char* pFuncName_in)
	{
		HookPtrMap::const_iterator Iter = m_HookMap.find(pFuncName_in);

		if (Iter != m_HookMap.end() && Iter->second != NULL)
			return InstallHook(Iter->second);

		return true;
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

			for(DWORD i = 8; i < OpCodesSize_in; i++)
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
}