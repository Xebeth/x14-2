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

// __int64 __fastcall sub_14099F950(__int64 a1, __int16 a2, __int64 a3, __int64 a4)
typedef int (WINAPI *fnSub14099F950)(LPVOID pThis_in_out, Windower::CHAT_MESSAGE_TYPE MessageType_in, Windower::StringNode* a2, Windower::StringNode* a3);
// __int64 __fastcall sub_14089CB00(__int64 a1, __int64 a2, __int64 a3, __int64 a4)
typedef int (WINAPI *fnSub14089CB00)(LPVOID pThis_in_out, Windower::StringNode* a2, Windower::StringNode* a3);
// int __fastcall sub_1409E7AA0(__int64 a1, __int64 a2, __int64 a3)
typedef int (WINAPI *fnSub1409E7AA0)(LPVOID pThis_in_out, Windower::StringNode* a2, Windower::StringNode* a3);
// int __fastcall sub_14097E430(__int64 a1, __int64 a2, unsigned int a3, unsigned int a4)
typedef int (WINAPI *fnSub1409E7870)(LPVOID pThis_in_out, Windower::StringNode* a2, unsigned int a3, unsigned int a4);

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
		static int WINAPI Sub1409E7870Hook(LPVOID pThis_in_out, Windower::StringNode* a2, unsigned int a3, unsigned int a4);
		static int WINAPI Sub14099F950Hook(LPVOID pThis_in_out, CHAT_MESSAGE_TYPE MessageType_in, Windower::StringNode* a2, Windower::StringNode* a3);
		static int WINAPI Sub14089CB00Hook(LPVOID pThis_in_out, Windower::StringNode* a2, Windower::StringNode* a3);
		static int WINAPI Sub1409E7AA0Hook(LPVOID pThis_in_out, Windower::StringNode* a2, Windower::StringNode* a3);

		fnSub1409E7870 m_pSub1409E7870Trampoline;
		fnSub14099F950 m_pSub14099F950Trampoline;
		fnSub14089CB00 m_pSub14089CB00Trampoline;
		fnSub1409E7AA0 m_pSub1409E7AA0Trampoline;
		//! calling context for the module hooks
		static CallingContext<TestCore> m_Context;
	};
}

#endif//__TEST_CORE_H__