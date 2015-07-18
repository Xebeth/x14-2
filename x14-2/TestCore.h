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

// signed __int64 __fastcall sub_140966890(__int64 a1, __int64 a2, __int64 a3)
typedef int (WINAPI *fnSub140966890)(LPVOID pThis_in_out, Windower::StringNode a2, __int64 a3);

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
		static int WINAPI Sub140966890Hook(LPVOID pThis_in_out, Windower::StringNode a2, __int64 a3);

		fnSub140966890 m_pSub140966890Trampoline;
		//! calling context for the module hooks
		static CallingContext<TestCore> m_Context;
	};
}

#endif//__TEST_CORE_H__