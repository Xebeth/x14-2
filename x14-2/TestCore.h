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
	typedef struct _StringNode StringNode;
}

// unsigned int __thiscall resetPlayerTarget(int this)
typedef int (WINAPI *fnSub59D550)(LPVOID pThis_in_out);

namespace Windower
{
	class WindowerEngine;

	//! \brief Test core module
	class TestCore : public WindowerCore
	{
	public:
		TestCore();

		// ICoreModule interface implementation
		void RegisterHooks(HookEngineLib::IHookManager &HookManager_in);
		void OnHookInstall(HookEngineLib::IHookManager &HookManager_in);

	protected:
		static int WINAPI Sub59D550Hook(LPVOID pThis_in_out);

		fnSub59D550 m_pSub59D550Trampoline;
		//! calling context for the module hooks
		static CallingContext<TestCore> m_Context;
	};
}

#endif//__TEST_CORE_H__