/**************************************************************************
	created		:	2011-06-02
	filename	: 	EasyHookManager.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Hook manager using EasyHook
**************************************************************************/
#ifndef __EASYHOOK_MANAGER_H__
#define __EASYHOOK_MANAGER_H__

#ifdef __USE_EASYHOOK

#ifndef _HOOK_MANAGER_SDK_INCLUDE_
	#error Please include the global header file 'HookEngine.h'
#endif

namespace HookEngineLib
{
	//! \brief Hook manager using EasyHook
	class EasyHookManager : public IHookManager
	{
	public:
		void Shutdown();

	protected:
		bool DestroyHook(const Hook *pHook_in);
		bool CreateHook(Hook *pHook_in_out);
		bool CommitTransaction();
		bool BeginTransaction();
		bool Initialize();

		//! EasyHook trace structure
		HOOK_TRACE_INFO m_hHook;
	};
}

#endif//__USE_EASYHOOK

#endif//__EASYHOOK_MANAGER_H__