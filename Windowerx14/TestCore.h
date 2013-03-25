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
	class WindowerEngine;

	//! \brief Test core module
	class TestCore : public WindowerCore
	{
	public:
		TestCore(WindowerEngine &Engine_in_out, HookEngine &HookManager_in_out);

		int sub568B90Hook(LPVOID pThis, char *pText, int TextLen, signed int a4);
		int sub8FFDA0Hook(LPVOID pThis, int a2, const char *a3, int a4);
		int sub48F4B0Hook(LPVOID pThis, char *pText, int TextLen);
		int InitStringNode2(LPVOID pThis, char *pText, int TextLen);
		bool IsCmd(LPVOID pThis, char *pText);

		// ICoreModule interface implementation
		void RegisterHooks(IHookManager &HookManager_in);
		void OnHookInstall(IHookManager &HookManager_in);

	protected:
		fn5E3810 m_p5E3810Trampoline;
		fn568B90 m_p568B90Trampoline;
		fnA73BE0 m_pA73BE0Trampoline;
		fn8FFDA0 m_p8FFDA0Trampoline;
		fn48F4B0 m_p48F4B0Trampoline;

		bool m_IsWindowerCmd;
	};
}

#endif//__TEST_CORE_H__