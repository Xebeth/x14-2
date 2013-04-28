/**************************************************************************
	created		:	2013-04-27
	filename	: 	Hook.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Hook structures
**************************************************************************/
#ifndef _HOOK_MANAGER_SDK_INCLUDE_
	#error Please include the global header file 'HookEngine.h'
#endif

#ifndef __HOOK_H__
#define __HOOK_H__

namespace HookEngineLib
{
	#define STACK_FRAME_OPCODES_SIZE	3UL
	#define JMP_OPCODE_SIZE				5UL
	#define TRAMPOLINE_OPCODES_SIZE		(STACK_FRAME_OPCODES_SIZE + JMP_OPCODE_SIZE)

	class MemberDetourAsm
	{	//! Byte codes used for class members hooking
		enum DETOUR_CLASS_OPCODES
		{
			OPCODE_NOP		= 0x90U,	//!< NOP
			OPCODE_JMP		= 0xE9U,	//!< JMP
			OPCODE_PUSH_EAX	= 0x50U,	//!< PUSH EAX
			OPCODE_PUSH_ECX	= 0x51U,	//!< PUSH ECX
			OPCODE_POP_EAX	= 0x58U,	//!< POP EAX
			OPCODE_POP_ECX	= 0x59U		//!< POP ECX
		};
	public:
		MemberDetourAsm();
		~MemberDetourAsm();

		bool SetDetour(LPBYTE pSrc_in, const LPBYTE pDst_in, DWORD OpCodesSize_in);
		bool SetRetour(LPBYTE pSrc_in, DWORD OpCodesSize_in);
		LPBYTE GetTrampoline() const;
	private:
		LPBYTE m_pTrampolineAsm;
		LPBYTE m_pOriginalAddr;
		DWORD m_TrampolineSize;
		bool m_bInstalled;
	};

	//! \brief Hook structure
	class Hook
	{
	public:
		
		Hook();
		Hook(const char* pFuncName_in, const char* pModuleName_in, LPVOID pOriginalFunc_in, LPVOID pHookFunc_in, DWORD dwOpCodeSize_in = 0);

		LPVOID			m_pOriginalFunc;		//!< function pointer to the target function
		LPVOID			m_pHookFunc;			//!< function pointer to the replacement function
		LPVOID			m_pTrampolineFunc;		//!< function pointer to the trampoline to the original function
		std::string		m_sModuleName;			//!< the name of the module containing the hooked function (i.e. Kernel32.dll)
		std::string		m_sFuncName;			//!< the name of the target function
		bool			m_bInstalled;			//!< flag specifying if the hook is in place
		DWORD			m_dwOpCodesSize;		//!< the size of the instructions being overwritten for class member hook
	};
}

#endif//__HOOK_H__
