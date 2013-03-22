/**************************************************************************
	created		:	2011-10-29
	filename	: 	HookManager.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Manager controlling the installation/removal of hooks
**************************************************************************/
#ifndef __HOOK_MANAGER_H__
#define __HOOK_MANAGER_H__

#include <string>
#include <hash_set>
#include <hash_map>

#include <SigScan.h>
#include <NonCopyable.h>

//! the hooks of a service
typedef stdext::hash_map<std::string, LPVOID> HookPointers;

namespace HookEngineLib
{
	//! \brief Hook structure
	class Hook
	{
	public:
		//! \brief Hook default constructor
		Hook() : m_pTrampolineFunc(NULL), m_pOriginalFunc(NULL), m_pHookFunc(NULL), m_bInstalled(false), m_dwOpCodesSize(0) {}
		/*! \brief Hook constructor
			\param[in] pFuncName_in : the name of the target function
			\param[in] pModuleName_in : the name of the module containing the hooked function (i.e. Kernel32.dll)
			\param[in] pOriginalFunc_in : function pointer to the target function
			\param[in] pHookFunc_in : function pointer to the replacement function
			\param[in] dwOpCodeSize_in : the size of the instructions being overwritten for class member hooks
		*/
		Hook(const char* pFuncName_in, const char* pModuleName_in, LPVOID pOriginalFunc_in, LPVOID pHookFunc_in, DWORD dwOpCodeSize_in = 0) :
			 m_sFuncName(pFuncName_in), m_sModuleName(pModuleName_in), m_pOriginalFunc(pOriginalFunc_in), m_pHookFunc(pHookFunc_in),
			 m_dwOpCodesSize(dwOpCodeSize_in), m_bInstalled(false), m_pTrampolineFunc(NULL) {}

		LPVOID			m_pOriginalFunc;		//!< function pointer to the target function
		LPVOID			m_pHookFunc;			//!< function pointer to the replacement function
		LPVOID			m_pTrampolineFunc;		//!< function pointer to the trampoline to the original function
		std::string		m_sModuleName;			//!< the name of the module containing the hooked function (i.e. Kernel32.dll)
		std::string		m_sFuncName;			//!< the name of the target function
		bool			m_bInstalled;			//!< flag specifying if the hook is in place
		DWORD			m_dwOpCodesSize;		//!< the size of the instructions being overwritten for class member hook
	};

	//! map of hooks
	typedef stdext::hash_map<std::string, Hook*> HookPtrMap;

	//! \brief Manager controlling the installation/removal of hooks
	class IHookManager : public NonCopyable
	{
	public:
		//! Byte codes used for class members hooking
		enum DETOUR_CLASS_OPCODES
		{
			OPCODE_NOP		= 0x90,		//!< NOP
			OPCODE_JMP		= 0xE9,		//!< JMP
			OPCODE_PUSH_EAX	= 0x50,		//!< PUSH EAX
			OPCODE_PUSH_ECX	= 0x51,		//!< PUSH ECX
			OPCODE_POP_EAX	= 0x58,		//!< POP EAX
			OPCODE_POP_ECX	= 0x59		//!< POP ECX
		};

		//! \brief IHookManager default constructor
		IHookManager() : NonCopyable(), m_bInit(false) {}
		~IHookManager();

		void RegisterHook(const char* pFuncName_in, const char* pModuleName_in, LPVOID pOriginalFunc_in,
						  LPVOID pHookFunc_in, DWORD dwOpCodeSize_in = 0);
		void RegisterHook(const char* pFuncName_in, const char* pModuleName_in, const char* pPattern_in,
						  int Offset_in, LPVOID pHookFunc_in, DWORD dwOpCodeSize_in = 0);
		void UnregisterHook(const char* pFuncName_in);
		LPVOID GetTrampolineFunc(const char* pFuncName_in) const;
		LPVOID GetOriginalFunc(const char* pFuncName_in) const;
		LPVOID GetHookFunc(const char* pFuncName_in) const;

		bool InstallHooks(HookPointers& HookList_in_out);
		bool UninstallHooks(HookPointers& HookList_in_out);
		void UnregisterHooks(HookPointers& HookList_in_out);

		LPVOID InstallHook(const char* pFuncName_in);
		bool UninstallHook(const char* pFuncName_in);
		bool IsHookInstalled(const char* pFuncName_in);
		bool IsHookRegistered(const char* pFuncName_in);

		bool UninstallRegisteredHooks();
		bool InstallRegisteredHooks();

		void RetourClassFunc(const LPBYTE pSrc_in, LPBYTE pTrampoline_in, DWORD OpCodesSize_in);
		LPVOID DetourClassFunc(LPBYTE pSrc_in, const LPBYTE pDst_in, DWORD OpCodesSize_in);		

	protected:
		virtual LPVOID FindModuleFunction(const char *pModuleName_in, const char *pFuncName_in);

		/*! \brief Destroys a hook by restoring the original function
			\param[in] pHook_in : the hook to destroy
			\return true if the hook was destroyed; false otherwise
		*/
		virtual bool DestroyHook(const Hook *pHook_in) =0;
		/*! \brief Creates a hook by patching the original function
		\param[in] pHook_in : the hook to create
		\return true if the hook was created; false otherwise
		*/
		virtual bool CreateHook(Hook *pHook_in_out) =0;
		/*! \brief Initializes the hooking process
			\return true if the initialization succeeded; false otherwise
		*/
		virtual bool Initialize() { return true; }
		/*! \brief Terminates the hooking process
			\return true if the termination succeeded; false otherwise
		*/
		virtual void Shutdown() { m_bInit = false; }

		/*! \brief Ends the installation process
		\return true if the termination succeeded; false otherwise
		*/
		virtual bool CommitTransaction() { return true; }
		/*! \brief Commits the installation process
			\return true if the commit succeeded; false otherwise
		*/
		virtual bool BeginTransaction() { return true; }

		void UnregisterHook(const HookPtrMap::iterator &Iter_in);
		bool UninstallHook(Hook *pHook_in_out);
		LPVOID InstallHook(Hook *pHook_in_out);

		//! map of installed hooks
		HookPtrMap m_HookMap;
		//! memory signature scanner
		SigScan::SigScan m_MemScan;
		//! flag specifying if the hook engine has been initialized
		bool m_bInit;

	private:
		typedef std::vector<LPBYTE> AsmData;
		//! vector of byte codes used for class member hooks
		AsmData m_AsmData;
	};
}

#endif//__HOOK_MANAGER_H__