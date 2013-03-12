/**************************************************************************
	created		:	2011-06-02
	filename	: 	MinHookManager.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Hook manager using MinHook
**************************************************************************/
#ifndef __MINHOOK_MANAGER_H__
#define __MINHOOK_MANAGER_H__

#ifdef __USE_MINHOOK

namespace HookEngineLib
{
	//! \brief Hook manager using MinHook
	class MinHookManager : public IHookManager
	{
	protected:
		bool DestroyHook(const Hook *pHook_in);
		bool CreateHook(Hook *pHook_in_out);
		bool CommitTransaction();
		bool BeginTransaction();
		bool Initialize();
		void Shutdown();
	};
}

#endif//__USE_MINHOOK

#endif//__MINHOOK_MANAGER_H__