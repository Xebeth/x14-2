/**************************************************************************
	created		:	2011-06-02
	filename	: 	DetoursManager.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Hook manager using Microsoft Detours 1.2
**************************************************************************/
#ifndef __DETOURS_MANAGER_H__
#define __DETOURS_MANAGER_H__

#ifdef __USE_DETOURS

#ifndef _HOOK_MANAGER_SDK_INCLUDE_
	#error Please include the global header file 'HookEngine.h'
#endif

namespace HookEngineLib
{
	class DetoursManager : public IHookManager
	{
	protected:
		LPVOID FindModuleFunction(const char *pModuleName_in, const char *pFuncName_in);

		bool DestroyHook(const Hook *pHook_in);
		bool CreateHook(Hook *pHook_in_out);
		bool CommitTransaction();
		bool BeginTransaction();
	};
}

#endif//__USE_DETOURS

#endif//__DETOURS_MANAGER_H__