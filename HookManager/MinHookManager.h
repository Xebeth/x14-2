/**************************************************************************
	created		:	2011-06-02
	filename	: 	MinHookManager.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Hook manager using MinHook
**************************************************************************/
#ifndef __MINHOOK_MANAGER_H__
#define __MINHOOK_MANAGER_H__

namespace HookEngineLib
{
	class MinHookManager : public IHookManager
	{
	public:
		MinHookManager() : m_bInit(false) {}

		virtual bool InstallRegisteredHooks();
		virtual bool UninstallRegisteredHooks();
	protected:
		virtual bool InstallHook(Hook *pHook);
		virtual bool UninstallHook(Hook *pHook);

		bool m_bInit;
	};
}

#endif//__MINHOOK_MANAGER_H__