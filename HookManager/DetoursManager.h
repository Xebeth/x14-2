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

namespace HookEngineLib
{
	class DetoursManager : public IHookManager
	{
	protected:
		bool DestroyHook(const Hook *pHook_in);
		bool CreateHook(Hook *pHook_in_out);
		bool CommitTransaction();
		bool BeginTransaction();
	};
}

#endif//__USE_DETOURS

#endif//__DETOURS_MANAGER_H__