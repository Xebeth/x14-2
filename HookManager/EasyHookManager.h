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
	//! \brief Hook manager using EasyHook
	class EasyHookManager : public IHookManager
	{
	public:
		//! EasyHookManager default constructor
		EasyHookManager() : m_bInit(false) {}

		virtual bool InstallRegisteredHooks();
		virtual bool UninstallRegisteredHooks();

	protected:
		virtual bool InstallHook(Hook *pHook_in_out);
		virtual bool UninstallHook(Hook *pHook_in_out);

		//! EasyHook trace structure
		HOOK_TRACE_INFO m_hHook;
	};
}

#endif//__EASYHOOK_MANAGER_H__