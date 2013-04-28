/**************************************************************************
	created		:	2011-06-15
	filename	: 	IATHookManager.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Hook manager using IAT Patcher
**************************************************************************/
#ifndef __IAT_HOOK_MANAGER_H__
#define __IAT_HOOK_MANAGER_H__

#ifdef __USE_IATHOOK

#ifndef _HOOK_MANAGER_SDK_INCLUDE_
	#error Please include the global header file 'HookEngine.h'
#endif

namespace HookEngineLib
{
	//! \brief Hook manager using IAT Patcher
	class IATHookManager : public IHookManager
	{
	public:
		//! \brief IATHookManager default constructor
		IATHookManager() : IHookManager(), m_hProcess(NULL) {}

		void Shutdown();

	protected:
		bool DestroyHook(const Hook *pHook_in);
		bool CreateHook(Hook *pHook_in_out);
		bool Initialize();

		//! the handle of the target process
		HMODULE m_hProcess;		
	};
}

#endif//__USE_IATHOOK

#endif//__IAT_HOOK_MANAGER_H__