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

		int Sub4357A0Hook(LPVOID pThis, DWORD a2, char * a3, int a4, int a5);
		
		// ICoreModule interface implementation
		void RegisterHooks(HookEngineLib::IHookManager &HookManager_in);
		void OnHookInstall(HookEngineLib::IHookManager &HookManager_in);
		
	protected:
		fnSub4357A0 m_pSub4357A0Trampoline;
	};
}

#endif//__TEST_CORE_H__