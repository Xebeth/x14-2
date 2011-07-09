#ifndef __HOOK_MANAGER_H__
#define __HOOK_MANAGER_H__

namespace HookEngineLib
{
	class Hook
	{
	public:
		Hook() { m_pTrampolineFunc = m_pOriginalFunc = m_pHookFunc = NULL; m_bInstalled = false; m_dwOpCodesSize = 0; }
		Hook(const char* pFuncName, const char* pModuleName, LPVOID pOriginalFunc, LPVOID pHookFunc, DWORD dwOpCodeSize = 0) :
			 m_sFuncName(pFuncName), m_sModuleName(pModuleName), m_pOriginalFunc(pOriginalFunc), m_pHookFunc(pHookFunc), m_dwOpCodesSize(dwOpCodeSize)
			 { m_bInstalled = false; m_pTrampolineFunc = NULL; }

		LPVOID			m_pOriginalFunc;		//!< the target function
		LPVOID			m_pHookFunc;			//!< the replacement function
		LPVOID			m_pTrampolineFunc;		//!< the trampoline to the original function
		std::string		m_sModuleName;			//!< the name of the module containing the hooked function (i.e. Kernel32.dll)
		std::string		m_sFuncName;			//!< the name of the target function
		bool			m_bInstalled;			//!< flag specifying if the hook is in place
		DWORD			m_dwOpCodesSize;		//!< offset used for class member hooking : the size of the instructions being overwritten
	};

	// map of hooks
	typedef stdext::hash_map<std::string, Hook*> HookPtrMap;
	// Hook map const iterators
	typedef HookPtrMap::const_reverse_iterator HookPtrConstReverseIterator;
	typedef HookPtrMap::const_iterator HookPtrConstIterator;
	// Hook map iterators
	typedef HookPtrMap::reverse_iterator HookPtrReverseIterator;
	typedef HookPtrMap::iterator HookPtrIterator;

	class IHookManager : public NonCopyable
	{
	public:
		enum DETOUR_CLASS_OPCODES
		{
			OPCODE_NOP		= 0x90,
			OPCODE_JMP		= 0xE9,
			OPCODE_PUSH_EAX	= 0x50,
			OPCODE_PUSH_ECX	= 0x51,
			OPCODE_POP_EAX	= 0x58,
			OPCODE_POP_ECX	= 0x59
		};

		virtual ~IHookManager();

		void RegisterHook(const char* pFuncName, const char* pModuleName, LPVOID pOriginalFunc, LPVOID pHookFunc, DWORD dwOpCodeSize = 0);
		void UnregisterHook(const char* pFuncName);
		LPVOID GetTrampolineFunc(const char* pFuncName);
		LPVOID GetOriginalFunc(const char* pFuncName);
		LPVOID GetHookFunc(const char* pFuncName);

		virtual bool InstallHook(const char* pFuncName);
		virtual bool UninstallHook(const char* pFuncName);

		virtual bool InstallRegisteredHooks() = 0;
		virtual bool UninstallRegisteredHooks() = 0;

		LPVOID DetourClassFunc(LPBYTE pSrc_in, const LPBYTE pDst_in, DWORD OpCodesSize_in);
		void RetourClassFunc(const LPBYTE pSrc_in, LPBYTE pTrampoline_in, DWORD OpCodesSize_in);
	protected:
		virtual bool InstallHook(Hook *pHook) = 0;
		virtual bool UninstallHook(Hook *pHook) = 0;
		virtual void UnregisterHook(HookPtrIterator Iter);

		HookPtrMap m_HookMap;

	private:
		typedef std::vector<LPBYTE> AsmData;
		AsmData m_AsmData;
	};
}

#endif//__HOOK_MANAGER_H__