/**************************************************************************
	created		:	2011-10-31
	filename	: 	TestCore.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Test core module
**************************************************************************/
#ifndef __TEST_CORE_H__
#define __TEST_CORE_H__

#define TESTING_MODULE "Testing"

namespace Windower
{
	class WindowerEngine;

	//! \brief Test core module
	class TestCore : public WindowerCore
	{
	public:
		TestCore();

		int Sub5D1C70Hook(LPVOID pThis, DWORD a2, char * a3, int a4, int a5);
		int Sub5E9E30Hook(LPVOID pThis, StringNode *a2);
		
		// ICoreModule interface implementation
		void RegisterHooks(HookEngineLib::IHookManager &HookManager_in);
		void OnHookInstall(HookEngineLib::IHookManager &HookManager_in);

	protected:
		fnSub5D1C70 m_pSub5D1C70Trampoline;
		fnSub5E9E30 m_pSub5E9E30Trampoline;
	};
}

#endif//__TEST_CORE_H__