/**************************************************************************
	created		:	2011-10-29
	filename	: 	HookManager.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Manager controlling the installation/removal of hooks
**************************************************************************/
#ifndef __HOOK_MANAGER_H__
#define __HOOK_MANAGER_H__

#ifndef _HOOK_MANAGER_SDK_INCLUDE_
	#error Please include the global header file 'HookEngine.h'
#endif

namespace SigScan
{
	class SigScan;
}

namespace HookEngineLib
{
	class MemberDetourAsm;
	class Hook;

	typedef stdext::hash_map<std::string, Hook*> HookPtrMap;

	//! \brief Manager controlling the installation/removal of hooks
	class IHookManager : public NonCopyable
	{
	public:
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

		LPVOID InstallHook(const char* pFuncName_in);
		bool UninstallHook(const char* pFuncName_in);
		bool IsHookInstalled(const char* pFuncName_in);
		bool IsHookRegistered(const char* pFuncName_in);

		bool InstallRegisteredHooks();
		bool UnregisterHooks();

		LPVOID DetourClassFunc(LPBYTE pSrc_in, const LPBYTE pDst_in, DWORD OpCodesSize_in);
		bool RetourClassFunc(const LPBYTE pSrc_in, DWORD OpCodesSize_in);

		SigScan::SigScan& GetSigScan();

		virtual void Shutdown();

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
		/*! \brief Ends the installation process
		\return true if the termination succeeded; false otherwise
		*/
		virtual bool CommitTransaction() { return true; }
		/*! \brief Commits the installation process
			\return true if the commit succeeded; false otherwise
		*/
		virtual bool BeginTransaction() { return true; }

		bool UnregisterHook(HookPtrMap::const_iterator &Iter_in_out);
		bool UninstallHook(Hook *pHook_in_out);
		LPVOID InstallHook(Hook *pHook_in_out);

		//! map of installed hooks
		HookPtrMap m_HookMap;
		//! memory signature scanner
		SigScan::SigScan m_MemScan;
		//! flag specifying if the hook engine has been initialized
		bool m_bInit;

	private:
		typedef std::map<LPBYTE, MemberDetourAsm*> MemberDetours;
		//! map of class member hooks
		MemberDetours m_AsmData;
	};
}

#endif//__HOOK_MANAGER_H__