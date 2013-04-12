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
	class WindowerEngine;

	//! \brief Test core module
	class TestCore : public WindowerCore
	{
	public:
		TestCore(WindowerEngine &Engine_in_out, HookEngine &HookManager_in_out);

		int Sub868970Hook(LPVOID pThis);
		int Sub83FD00Hook(LPVOID pThis);
		
		// ICoreModule interface implementation
		void RegisterHooks(HookEngineLib::IHookManager &HookManager_in);
		void OnHookInstall(HookEngineLib::IHookManager &HookManager_in);

	protected:
		fnSub868970 m_pSub868970Trampoline;
		fnSub83FD00 m_pSub83FD00Trampoline;
	};
}

#endif//__TEST_CORE_H__