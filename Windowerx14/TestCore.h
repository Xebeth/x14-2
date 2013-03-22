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

	//! \brief Test core module
	class TestCore : public WindowerCore
	{
	public:
		TestCore(WindowerEngine &Engine_in_out, HookEngine &HookManager_in_out);

		int sub5668C0Hook(LPVOID pThis, char *pText, int TextLen, signed int a4);
		int sub8FDF60Hook(LPVOID pThis, int a2, const char *a3, int a4);
		int sub5E0DF0Hook(LPVOID pThis, char *pText, int TextLen);
		bool subA73810Hook(LPVOID pThis, char *pText);
		int sub490020Hook(LPVOID pThis, char *pText);		

		// ICoreModule interface implementation
		void RegisterHooks(IHookManager &HookManager_in);
		void OnHookInstall(IHookManager &HookManager_in);

	protected:
		fn5E0DF0 m_p5E0DF0Trampoline;
		fn490020 m_p490020Trampoline;
		fn5668C0 m_p5668C0Trampoline;
		fnA73810 m_pA73810Trampoline;
		fn8FDF60 m_p8FDF60Trampoline;

		bool m_IsWindowerCmd;
	};
}

#endif//__TEST_CORE_H__