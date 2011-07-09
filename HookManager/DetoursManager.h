/**************************************************************************
	created		:	2011-06-02
	filename	: 	DetoursManager.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Hook manager using Microsoft Detours 1.2
**************************************************************************/
#ifndef __DETOURS_MANAGER_H__
#define __DETOURS_MANAGER_H__

namespace HookEngineLib
{
	class DetoursManager : public IHookManager
	{
	public:
		virtual bool InstallRegisteredHooks();
		virtual bool UninstallRegisteredHooks();
	protected:
		virtual bool InstallHook(Hook *pHook);
		virtual bool UninstallHook(Hook *pHook);
	};
}

#endif//__DETOURS_MANAGER_H__