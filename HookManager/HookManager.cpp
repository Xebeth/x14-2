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
	/*! \brief destructor */
	IHookManager::~IHookManager()
	{
		HookPtrIterator Iter;

		for (Iter = m_HookMap.begin(); Iter != m_HookMap.end(); ++Iter)
			UnregisterHook(Iter);

		AsmData::iterator AsmIter;

		for (AsmIter = m_AsmData.begin(); AsmIter != m_AsmData.end(); ++AsmIter)
			free(*AsmIter);
	}

	void IHookManager::RegisterHook(const char* pFuncName, const char* pModuleName, LPVOID pOriginalFunc, LPVOID pHookFunc, DWORD dwOpCodeSize)
	{
		HookPtrIterator Iter = m_HookMap.find(pFuncName);

		// clean up the previous instance
		if (Iter != m_HookMap.end() && Iter->second != NULL)
			UninstallHook(Iter->second);

		m_HookMap[pFuncName] = new Hook(pFuncName, pModuleName, pOriginalFunc, pHookFunc, dwOpCodeSize);
	}

	void IHookManager::UnregisterHook(const char* pFuncName)
	{
		HookPtrIterator Iter = m_HookMap.find(pFuncName);

		if (Iter != m_HookMap.end())
			UnregisterHook(Iter);
	}

	void IHookManager::UnregisterHook(HookPtrIterator Iter)
	{
		if (Iter != m_HookMap.end() && Iter->second != NULL)
		{
			if (Iter->second->m_bInstalled)
				UninstallHook(Iter->second);

			delete Iter->second;
			Iter->second = NULL;
		}
	}

	LPVOID IHookManager::GetOriginalFunc(const char* pFuncName)
	{
		HookPtrIterator Iter = m_HookMap.find(pFuncName);

		if (Iter != m_HookMap.end() && Iter->second != NULL)
			return Iter->second->m_pOriginalFunc;
		else
			return NULL;
	}

	LPVOID IHookManager::GetHookFunc(const char* pFuncName)
	{
		HookPtrIterator Iter = m_HookMap.find(pFuncName);

		if (Iter != m_HookMap.end() && Iter->second != NULL)
			return Iter->second->m_pHookFunc;
		else
			return NULL;
	}

	/*! \brief 
	\param[] pFuncName : 
	*/
	LPVOID IHookManager::GetTrampolineFunc(const char* pFuncName)
	{
		HookPtrIterator Iter = m_HookMap.find(pFuncName);

		if (Iter != m_HookMap.end() && Iter->second != NULL)
			return Iter->second->m_pTrampolineFunc;
		else
			return NULL;
	}

	/*! \brief 
	\param[] pFuncName : 
	*/
	bool IHookManager::InstallHook(const char* pFuncName)
	{
		HookPtrIterator Iter = m_HookMap.find(pFuncName);

		if (Iter != m_HookMap.end() && Iter->second != NULL)
			return InstallHook(Iter->second);

		return true;
	}

	/*! \brief 
	\param[] pFuncName : 
	*/
	bool IHookManager::UninstallHook(const char* pFuncName)
	{
		HookPtrIterator Iter = m_HookMap.find(pFuncName);

		if (Iter != m_HookMap.end() && Iter->second != NULL)
			return UninstallHook(Iter->second);

		return true;
	}


	/*! \brief Hooks a class member with an __stdcall function by pushing ECX (this) on the stack.
		\param[in] pSrc_in : the address of the function to be hooked
		\param[in] pDst_in : an __stdcall hook function
		\param[in] OpCodesSize_in : the size of the first opcodes of the original function which size is greater than 8.
		\return the trampoline (detoured) function pointer
	*/
	void* IHookManager::DetourClassFunc(LPBYTE pSrc_in, const LPBYTE pDst_in, DWORD OpCodesSize_in)
	{
		LPBYTE pTrampoline = (LPBYTE)malloc(OpCodesSize_in+8);

		DWORD dwPageAccess;
		VirtualProtect(pSrc_in, OpCodesSize_in, PAGE_READWRITE, &dwPageAccess);
		memcpy(pTrampoline+3, pSrc_in, OpCodesSize_in);

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

		for(DWORD i = 8; i < OpCodesSize_in; i++ )
			pSrc_in[i] = OPCODE_NOP;							// NOP

		VirtualProtect(pSrc_in, OpCodesSize_in, dwPageAccess, &dwPageAccess);

		m_AsmData.push_back(pTrampoline);

		return pTrampoline;
	}

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