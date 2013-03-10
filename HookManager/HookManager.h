/**************************************************************************
	created		:	2011-10-29
	filename	: 	HookManager.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Manager controlling the installation/removal of hooks
**************************************************************************/
#ifndef __HOOK_MANAGER_H__
#define __HOOK_MANAGER_H__

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

		virtual ~IHookManager();

		void RegisterHook(const char* pFuncName_in, const char* pModuleName_in, LPVOID pOriginalFunc_in,
						  LPVOID pHookFunc_in, DWORD dwOpCodeSize_in = 0);
		void UnregisterHook(const char* pFuncName_in);
		LPVOID GetTrampolineFunc(const char* pFuncName_in) const;
		LPVOID GetOriginalFunc(const char* pFuncName_in) const;
		LPVOID GetHookFunc(const char* pFuncName_in) const;

		virtual bool InstallHook(const char* pFuncName_in);
		virtual bool UninstallHook(const char* pFuncName_in);

		/*! \brief Installs all the hooks currently registered in the manager
			\return true if all the hooks were installed successfully; false otherwise
		*/
		virtual bool InstallRegisteredHooks() =0;
		/*! \brief Uninstalls all the hooks currently registered in the manager
			\return true if all the hooks were uninstalled successfully; false otherwise
		*/
		virtual bool UninstallRegisteredHooks() =0;

		LPVOID DetourClassFunc(LPBYTE pSrc_in, const LPBYTE pDst_in, DWORD OpCodesSize_in);
		void RetourClassFunc(const LPBYTE pSrc_in, LPBYTE pTrampoline_in, DWORD OpCodesSize_in);

	protected:
		/*! \brief Installs the specified hook
			\param[in] pHook_in : the hook to install
			\return true if the hook was installed successfully; false otherwise
		*/
		virtual bool InstallHook(Hook *pHook_in) =0;
		/*! \brief Uninstalls the specified hook
			\param[in] pHook_in : the hook to install
			\return true if the hook was uninstalled successfully; false otherwise
		*/
		virtual bool UninstallHook(Hook *pHook_in) =0;
		virtual void UnregisterHook(const HookPtrMap::iterator &Iter_in);

		//! map of installed hooks
		HookPtrMap m_HookMap;
		//! flag specifying if the hook engine has been initialized
		bool m_bInit;

	private:
		typedef std::vector<LPBYTE> AsmData;
		//! vector of byte codes used for class member hooks
		AsmData m_AsmData;
	};
}

#endif//__HOOK_MANAGER_H__