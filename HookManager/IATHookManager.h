/**************************************************************************
	created		:	2011-06-15
	filename	: 	IATHookManager.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Hook manager using IAT Patcher
**************************************************************************/
#ifndef __IAT_HOOK_MANAGER_H__
#define __IAT_HOOK_MANAGER_H__

namespace HookEngineLib
{
	class IATHookManager : public IHookManager
	{
	public:
		IATHookManager() : m_bInit(false), m_hProcess(NULL) {}

		virtual bool InstallRegisteredHooks();
		virtual bool UninstallRegisteredHooks();
	protected:
		virtual bool InstallHook(Hook *pHook);
		virtual bool UninstallHook(Hook *pHook);

		HMODULE m_hProcess;
		bool m_bInit;
	};
}
#endif//__IAT_HOOK_MANAGER_H__