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

typedef struct _StringNode StringNode;

// int __thiscall sub_5E9E30(int this, int a2)
typedef int (WINAPI *fnSub5E9E30)(LPVOID pThis, StringNode * a2);
// int __thiscall sub_5D1C70(int this, int a2, int a3, int a4, int a5)
typedef int (WINAPI *fnSub5D1C70)(LPVOID pThis, DWORD a2, char *a3, int a4, int a5);

namespace Windower
{
	class WindowerEngine;

	//! \brief Test core module
	class TestCore : public WindowerCore
	{
	public:
		TestCore();

		static int WINAPI Sub5D1C70Hook(LPVOID pThis, DWORD a2, char * a3, int a4, int a5);
		static int WINAPI Sub5E9E30Hook(LPVOID pThis, StringNode *a2);
		
		// ICoreModule interface implementation
		void RegisterHooks(HookEngineLib::IHookManager &HookManager_in);
		void OnHookInstall(HookEngineLib::IHookManager &HookManager_in);

	protected:
		fnSub5D1C70 m_pSub5D1C70Trampoline;
		fnSub5E9E30 m_pSub5E9E30Trampoline;
		//! calling context for the module hooks
		static CallingContext<TestCore> m_Context;
	};
}

#endif//__TEST_CORE_H__