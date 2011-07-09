/**************************************************************************
	created		:	2011-06-02
	filename	: 	EasyHookManager.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Hook manager using EasyHook
**************************************************************************/
#ifndef __EASYHOOK_MANAGER_H__
#define __EASYHOOK_MANAGER_H__

namespace HookEngineLib
{
	class EasyHookManager : public IHookManager
	{
	public:
		EasyHookManager() : m_bInit(false) {}

		virtual bool InstallRegisteredHooks();
		virtual bool UninstallRegisteredHooks();
	protected:
		virtual bool InstallHook(Hook *pHook);
		virtual bool UninstallHook(Hook *pHook);

		HOOK_TRACE_INFO m_hHook;
		bool m_bInit;
	};
}

#endif//__EASYHOOK_MANAGER_H__