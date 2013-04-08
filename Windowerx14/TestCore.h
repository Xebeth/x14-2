/**************************************************************************
	created		:	2011-10-31
	filename	: 	TestCore.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Test core module
**************************************************************************/
#ifndef __TEST_CORE_H__
#define __TEST_CORE_H__

namespace Windower
{
	typedef HookEngineLib::IHookManager IHookManager;
	class WindowerEngine;

	//! \brief Test core module
	class TestCore : public WindowerCore
	{
	public:
		TestCore(WindowerEngine &Engine_in_out, HookEngine &HookManager_in_out);

		int Sub69C130Hook(LPVOID pThis);
		
		// ICoreModule interface implementation
		void RegisterHooks(IHookManager &HookManager_in);
		void OnHookInstall(IHookManager &HookManager_in);

	protected:
		fnSub69C130 m_pSub69C130Trampoline;
	};
}

#endif//__TEST_CORE_H__