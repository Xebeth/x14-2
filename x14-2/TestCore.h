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

// unsigned int __thiscall sub_AD1390(int this, const void *a2, size_t a3)
typedef int (WINAPI *fnSubAD1390)(LPVOID pThis_in_out, Windower::StringNode* pCmd_in_out, LPVOID pUnknown_in);

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
		static int WINAPI SubAD1390Hook(LPVOID pThis_in_out, Windower::StringNode* pCmd_in_out, LPVOID pUnknown_in);

		fnSubAD1390 m_pSubAD1390Trampoline;
		//! calling context for the module hooks
		static CallingContext<TestCore> m_Context;
	};
}

#endif//__TEST_CORE_H__