/**************************************************************************
	created		:	2013-04-27
	filename	: 	Hook.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "Hook.h"

namespace HookEngineLib
{
	//! \brief Hook default constructor
	Hook::Hook() : m_pTrampolineFunc(NULL), m_pOriginalFunc(NULL),
		m_pHookFunc(NULL), m_bInstalled(false), m_dwOpCodesSize(0) {}

	/*! \brief Hook constructor
		\param[in] pFuncName_in : the name of the target function
		\param[in] pModuleName_in : the name of the module containing the hooked function (i.e. Kernel32.dll)
		\param[in] pOriginalFunc_in : function pointer to the target function
		\param[in] pHookFunc_in : function pointer to the replacement function
		\param[in] dwOpCodeSize_in : the size of the instructions being overwritten for class member hooks
	*/
	Hook::Hook(const char* pFuncName_in, const char* pModuleName_in,
			   LPVOID pOriginalFunc_in, LPVOID pHookFunc_in, DWORD dwOpCodeSize_in)
		: m_sFuncName(pFuncName_in), m_sModuleName(pModuleName_in), m_pOriginalFunc(pOriginalFunc_in),
		  m_pHookFunc(pHookFunc_in), m_dwOpCodesSize(dwOpCodeSize_in), m_bInstalled(false), m_pTrampolineFunc(NULL) {}

	//! \brief MemberDetourAsm default constructor
	MemberDetourAsm::MemberDetourAsm()
		: m_pTrampolineAsm(NULL),
		  m_pOriginalAddr(NULL),
		  m_TrampolineSize(0UL) {}

	//! \brief MemberDetourAsm destructor
	MemberDetourAsm::~MemberDetourAsm()
	{
		if (m_pTrampolineAsm != NULL)
		{
			free(m_pTrampolineAsm);
			m_pTrampolineAsm = NULL;
		}
	}

	/*! \brief Initializes the detour
		\param[in] pSrc_in : the address of the function to be hooked
		\param[in] pDst_in : an __stdcall hook function
		\param[in] OpCodesSize_in : the size of the first byte of the original function
									it must include the size of the byte codes comprising
									the instructions so at least 8 bytes are overwritten
	*/
	bool MemberDetourAsm::SetDetour(LPBYTE pSrc_in, const LPBYTE pDst_in, DWORD OpCodesSize_in)
	{
		if (pSrc_in != NULL && pDst_in != NULL && OpCodesSize_in >= TRAMPOLINE_OPCODES_SIZE)
		{
			m_TrampolineSize = (OpCodesSize_in + TRAMPOLINE_OPCODES_SIZE);
			
			m_pOriginalAddr = pSrc_in;
			m_pTrampolineAsm = (LPBYTE)malloc(m_TrampolineSize * sizeof(BYTE));
			memset(m_pTrampolineAsm, OPCODE_NOP, m_TrampolineSize * sizeof(BYTE));

			if (m_pTrampolineAsm != NULL)
			{
				DWORD JmpOpOffset = OpCodesSize_in + STACK_FRAME_OPCODES_SIZE;	// offset of the JMP instruction
				DWORD JmpAddrOffset = JmpOpOffset + 1;							// offset of the JMP address operand
				// relative 32-bit JMP
				DWORD HookRelAddr = (pSrc_in + OpCodesSize_in)							// Source address
								  - (m_pTrampolineAsm + JmpOpOffset + JMP_OPCODE_SIZE);	// Destination address
				DWORD TrampolineRelAddr = pDst_in										// Source address
										- (pSrc_in + TRAMPOLINE_OPCODES_SIZE);			// Destination address

				// write the hook code
				m_pTrampolineAsm[0] = OPCODE_POP_EAX;							// POP EAX
				m_pTrampolineAsm[1] = OPCODE_POP_ECX;							// POP ECX
				m_pTrampolineAsm[2] = OPCODE_PUSH_EAX;							// PUSH EAX
				// copy the original code after the jump
				memcpy_s(m_pTrampolineAsm + STACK_FRAME_OPCODES_SIZE, OpCodesSize_in,
						 pSrc_in, OpCodesSize_in);
				m_pTrampolineAsm[JmpOpOffset] = OPCODE_JMP;						// JMP
				// jump offset
				*(DWORD*)(m_pTrampolineAsm + JmpAddrOffset) = HookRelAddr;

				// detour the source function call : push ECX (this) on the stack
				pSrc_in[0] = OPCODE_POP_EAX;							// POP EAX
				pSrc_in[1] = OPCODE_PUSH_ECX;							// PUSH ECX
				pSrc_in[2] = OPCODE_PUSH_EAX;							// PUSH EAX
				pSrc_in[3] = OPCODE_JMP;								// JMP
				// jump offset
				*(DWORD*)(pSrc_in + 4) = TrampolineRelAddr;
				// NOP any opcode that wasn't overwritten by the hook
				memset(pSrc_in + TRAMPOLINE_OPCODES_SIZE, OPCODE_NOP, OpCodesSize_in - TRAMPOLINE_OPCODES_SIZE);

				return true;
			}
		}

		return false;
	}

	/*! \brief Restores the original code
		\param[in] pSrc_in : the address of the original code
		\param[in] OpCodesSize_in : the size of the first byte of the original function
									it must include the size of the byte codes comprising
									the instructions so at least 8 bytes are overwritten
	*/
	bool MemberDetourAsm::SetRetour(LPBYTE pSrc_in, DWORD OpCodesSize_in)
	{
		if (m_pTrampolineAsm != NULL && m_pOriginalAddr == pSrc_in
		 && OpCodesSize_in == (m_TrampolineSize - TRAMPOLINE_OPCODES_SIZE))
		{
			LPBYTE pSrcOpcodes = m_pTrampolineAsm + STACK_FRAME_OPCODES_SIZE;

			// restore the original code
			memcpy_s(pSrc_in, OpCodesSize_in, pSrcOpcodes, OpCodesSize_in);

			return true;
		}

		return false;
	}

	/*! \brief Retrieves the trampoline
		\return a pointer to the trampoline
	*/
	LPBYTE MemberDetourAsm::GetTrampoline() const
	{ return m_pTrampolineAsm; }
}