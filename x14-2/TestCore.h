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

// int __thiscall sub_B5FD90(int this, char *DstBuf, size_t SizeInBytes)
typedef int (WINAPI *fnSubB5FD90)(LPVOID pThis, char *pDstBuf, size_t SizeInBytes);

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
		static int WINAPI SubB5FD90Hook(LPVOID pThis, char *pDstBuf, size_t SizeInBytes);

		fnSubB5FD90 m_pSubB5FD90Trampoline;
		//! calling context for the module hooks
		static CallingContext<TestCore> m_Context;
	};
}

#endif//__TEST_CORE_H__